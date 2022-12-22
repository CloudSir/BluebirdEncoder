/*
 * @Author: CloudSir
 * @Github: https://github.com/CloudSir/BluebirdEncoder
 * @Date: 2022-12-21 20:36:22
 * @LastEditTime: 2022-12-22 15:03:31
 * @LastEditors: CloudSir
 * @Description: 青鸟编码器
 */

#include "bluebird_encoder.h"

/**
 * 将要发送的数据编码成字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {uint16_t} *data 要传输的数据的数组
 * @param {uint8_t} length 要传输的数据长度
 * @param {uint8_t} type 1:int16; 0:uint16
 * @return {uint_8} 1:成功；0：失败
 */
uint8_t pack_data(Data_t *data_s, uint16_t *data, uint8_t length, uint8_t type)
{
    data_s->head1 = 0xEB;
    data_s->head2 = 0x90;
    data_s->tail = 0xBE;

    data_s->length = length;
    data_s->type = type;

    // 填充数据
    for (int i = 0; i < length; i++)
    {
        if (!type)
            data_s->data_union.data_u16[i] = data[i];
        else
            data_s->data_union.data_i16[i] = (int16_t)data[i];
    }

    // 计算校验和
    for (int i = 0; i < length * 2; i++)
    {
        data_s->check_sum += data_s->data_union.buffer_data[i];
    }

    return 1;
}

/**
 * 发送编码后的字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {(*uart_send)(uint8_t *, int)} 串口发送函数的指针
 * @return {*}
 */
uint8_t send_data(Data_t *data_s, void (*uart_send)(uint8_t *, int))
{
    uint8_t D1 = (data_s->length << 1) | data_s->type;

    uart_send(&data_s->head1, 1);
    uart_send(&data_s->head2, 1);
    uart_send(&D1, 1);
    uart_send(data_s->data_union.buffer_data, data_s->length * 2);
    uart_send(&data_s->check_sum, 1);
    uart_send(&data_s->tail, 1);
}

/**
 * 将接收的字节数组解析为数据
 * @param {uint8_t} data 接收的字节
 * @param {Data_t} *data_s 数据结构体指针
 * @return {uint8_t} 1:接收成功；0：接收未完成；
 */
uint8_t unpack_data(Data_t *data_s, uint8_t data)
{
    static uint8_t state = 0;
    static uint8_t i = 0;

    // 判断当前状态
    switch (state)
    {
    case 0: // 检查帧头

        if(i == 0)
        {
            data_s->head1 = 0xEB;
            data_s->head2 = 0x90;
            data_s->tail = 0xBE;

            if (data_s->head1 == data) // 检查第一帧头
            {
                i += 1;
            }
        }
        else if(i == 1)
        {
            i = 0;
            if (data_s->head2 == data) // 检查第二帧头
            {
                state++; // 进入下一状态
            }
        }
        
        break;

    case 1: // 接收数据的长度变量; 接收数据类型变量;
        data_s->length = data >> 1;
        data_s->type = data & 0x01;
        state++;
        break;

    case 2: // 接收数据
        data_s->data_union.buffer_data[i++] = data;
        if (i >= (2 * data_s->length))
        {
            state++;
        }
        break;

    case 3: // 接收数据校验和
        data_s->check_sum = data;
        state++;
        break;

    case 4: // 检查帧尾
        if (data_s->tail == data)
        {

            uint8_t check_sum = 0;
            // 验证校验和是否正确
            for (int i = 0; i < sizeof(uint16_t) * data_s->length; i++)
            {
                check_sum += data_s->data_union.buffer_data[i];
            }

            if (check_sum == data_s->check_sum)
            {
                // 接收成功

                state = 0;
                return 1;
            }
            else
            {
                // 校验和错误
                state = 0;
            }
        }
        else
        {
            // 帧尾错误
            state = 0;
        }
        break;

    default:
        state = 0;
        break;
    }

    return 0;
}
