/*
 * @Author: CloudSir
 * @Github: https://github.com/CloudSir
 * @Date: 2022-12-21 20:35:20
 * @LastEditTime: 2022-12-24 14:41:06
 * @LastEditors: CloudSir
 * @Description: 青鸟编码器头文件
 */

#ifndef __BLUEBIRD_PROTO__
#define __BLUEBIRD_PROTO__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint8_t head1;                   // 帧头
    uint8_t head2;                   // 帧头
    uint8_t length : 7;             // 解码后的数据长度，范围 0-127
    uint8_t type : 1;               // 数据类型
    union
    {
        uint8_t buffer_data[254];   // 编码后的字节数组
        uint16_t data_u16[127];     // 解码后的uint16类型数组
        int16_t data_i16[127];      // 解码后的int16类型数组
    } data_union;  
    uint8_t check_sum;              // 校验和
    uint8_t tail;                   // 帧尾

    uint8_t __i;
    uint8_t __state;
} Data_t;

/**
 * 将要发送的数据编码成字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {uint16_t} *data 要传输的数据的数组
 * @param {uint8_t} length 要传输的数据长度
 * @param {uint8_t} type 1:int16; 0:uint16
 * @return {uint_8} 1:成功；0：失败
 */
uint8_t bluebird_pack(Data_t *data_s, uint16_t *data, uint8_t length, uint8_t type);

/**
 * 发送编码后的字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {(*uart_send)(uint8_t *, int)} 串口发送函数的指针
 * @return {*}
 */
uint8_t bluebird_send(Data_t *data_s, void (*uart_send)(uint8_t *, int));

/**
 * 将接收的字节数组解析为数据
 * @param {uint8_t} data 接收的字节
 * @param {Data_t} *data_s 数据结构体指针
 * @return {uint8_t} 1:接收成功；0：接收未完成；
 */
uint8_t bluebird_unpack(Data_t *data_s, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
