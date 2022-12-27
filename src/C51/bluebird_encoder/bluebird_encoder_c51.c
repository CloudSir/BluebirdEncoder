/*
 * @Author: CloudSir
 * @Github: https://github.com/CloudSir
 * @Date: 2022-12-27 10:18:08
 * @LastEditTime: 2022-12-27 10:47:35
 * @LastEditors: CloudSir
 * @Description: 
 */

#include "bluebird_encoder_c51.h"

/**
 * 交换u16数组对应字节数组的字节序
 * @param {unsigned char} *bytes
 * @param {int} length
 * @return {*}
 */
static void swap_u16_seq(unsigned char *bytes, int length)
{
    unsigned char tmp;
    int i = 0;
    for(i = 0; i < length; i+=2)
    {
        tmp =  bytes[i];
        bytes[i] = bytes[i+1];
        bytes[i+1] = tmp;
    }
}

/**
 * 交换数据结构体中要传输数据的字节序
 * @param {Data_t} *data_s 数据结构体
 * @return {*}
 */
static void swap_data_seq(Data_t *data_s)
{
    swap_u16_seq(data_s->data_union.buffer_data, data_s->length * 2);
}

/**
 * 将要发送的数据编码成字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {unsigned short} *data_ 要传输的数据的数组
 * @param {unsigned char} length 要传输的数据长度
 * @param {unsigned char} type 1:int16; 0:uint16
 * @return {uint_8} 1:成功；0：失败
 */
unsigned char bluebird_pack(Data_t *data_s, unsigned short *data_, unsigned char length, unsigned char type)
{
    int i = 0;

    data_s->head1 = 0xEB;
    data_s->head2 = 0x90;
    data_s->tail = 0xBE;

    data_s->length = length;
    data_s->type = type;

    // 填充数据
    for (i = 0; i < length; i++)
    {
        if (!type)
            data_s->data_union.data_u16[i] = data_[i];
        else
            data_s->data_union.data_i16[i] = (short)data_[i];
    }

    // 交换字节序
    swap_data_seq(data_s);

    // 计算校验和
    for (i = 0; i < length * 2; i++)
    {
        data_s->check_sum += data_s->data_union.buffer_data[i];
    }

    return 1;
}

/**
 * 发送编码后的字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {(*uart_send)(unsigned char *, int)} 串口发送函数的指针
 * @return {*}
 */
unsigned char bluebird_send(Data_t *data_s, void (*uart_send)(unsigned char *, int))
{
    unsigned char D1 = (data_s->length << 1) | data_s->type;

    uart_send(&data_s->head1, 1);
    uart_send(&data_s->head2, 1);
    uart_send(&D1, 1);
    uart_send(data_s->data_union.buffer_data, data_s->length * 2);
    uart_send(&data_s->check_sum, 1);
    uart_send(&data_s->tail, 1);

    return 1;
}

/**
 * 将接收的字节数组解析为数据
 * @param {unsigned char} data_ 接收的字节
 * @param {Data_t} *data_s 数据结构体指针
 * @return {unsigned char} 1:接收成功；0：接收未完成；
 */
unsigned char bluebird_unpack(Data_t *data_s, unsigned char data_)
{

    unsigned char check_sum = 0;  // 计算出的校验和
    int i = 0;                    // for 循环计数变量

    // 判断当前状态
    switch (data_s-> __state)
    {
    
    default: // 初始化状态码
        data_s-> __state = 0;
        data_s-> __i = 0;

    case 0: // 检查帧头

        if(data_s-> __i == 0)
        {
            if (data_s->head1 == data_) // 检查第一帧头
            {
                data_s-> __i += 1;
            }
        }
        else if(data_s-> __i == 1)
        {
            data_s-> __i = 0;
            if (data_s->head2 == data_) // 检查第二帧头
            {
                data_s-> __state++; // 进入下一状态
            }
        }
        
        break;

    case 1: // 接收数据的长度变量; 接收数据类型变量;
        data_s->length = data_ >> 1;
        data_s->type = data_ & 0x01;
        data_s-> __state++;
        break;

    case 2: // 接收数据
        data_s->data_union.buffer_data[data_s-> __i++] = data_;
        if (data_s-> __i >= (2 * data_s->length))
        {
            data_s-> __state++;
        }
        break;

    case 3: // 接收数据校验和
        data_s->check_sum = data_;
        data_s-> __state++;
        break;

    case 4: // 检查帧尾
        if (data_s->tail == data_)
        {
            // 验证校验和是否正确
            for (i = 0; i < sizeof(unsigned short) * data_s->length; i++)
            {
                check_sum += data_s->data_union.buffer_data[i];
            }

            if (check_sum == data_s->check_sum)
            {
                // 接收成功
                data_s-> __state = 0;
                data_s-> __i = 0;

                // 交换字节序
                swap_data_seq(data_s);

                return 1;
            }
            else
            {
                // 校验和错误
            }
        }
        else
        {
            // 帧尾错误
        }

        // 初始化状态码
        data_s-> __state = 0;
        data_s-> __i = 0;

        break;
    }

    return 0;
}

/**
 * 数据结构体初始化函数
 * @param {Data_t} *data_s 数据结构体
 * @return {*}
 */
void bluebird_init(Data_t *data_s)
{
    data_s-> head1 = 0xEB;
    data_s-> head2 = 0x90;
    data_s-> length = 0;
    data_s-> type = 0;
    memset(data_s-> data_union.buffer_data, 0, BLUEBIRD_MAX_DATA_LENGTH * 2);
    data_s-> check_sum = 0;
    data_s-> tail = 0xBE;
    data_s-> __state = 0;
    data_s-> __i = 0;
}
