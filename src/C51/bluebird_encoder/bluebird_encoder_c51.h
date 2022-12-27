/*
 * @Author: CloudSir
 * @Github: https://github.com/CloudSir
 * @Date: 2022-12-27 10:18:19
 * @LastEditTime: 2022-12-27 10:53:57
 * @LastEditors: CloudSir
 * @Description: 
 */

#ifndef __BLUEBIRD_ENCODER__
#define __BLUEBIRD_ENCODER__

#ifdef __cplusplus
extern "C" {
#endif

#define BLUEBIRD_MAX_DATA_LENGTH 127  // 接收/发送数据的最大长度，正整数，最多127

typedef struct
{
    unsigned char head1;                  // 帧头
    unsigned char head2;                  // 帧头
    unsigned char length : 7;             // 解码后的数据长度，范围 0-127
    unsigned char type : 1;               // 数据类型
    union
    {
        unsigned char buffer_data[BLUEBIRD_MAX_DATA_LENGTH * 2];  // 编码后的字节数组
        unsigned short data_u16[BLUEBIRD_MAX_DATA_LENGTH];        // 解码后的uint16类型数组
        short data_i16[BLUEBIRD_MAX_DATA_LENGTH];         // 解码后的int16类型数组
    } data_union;  
    unsigned char check_sum;              // 校验和
    unsigned char tail;                   // 帧尾

    unsigned char __i;
    unsigned char __state;
} Data_t;

/**
 * 将要发送的数据编码成字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {unsigned short} *data_ 要传输的数据的数组
 * @param {unsigned char} length 要传输的数据长度
 * @param {unsigned char} type 1:int16; 0:uint16
 * @return {uint_8} 1:成功；0：失败
 */
unsigned char bluebird_pack(Data_t *data_s, unsigned short *data_, unsigned char length, unsigned char type);

/**
 * 发送编码后的字节数组
 * @param {Data_t} *data_s 数据结构体指针
 * @param {(*uart_send)(unsigned char *, int)} 串口发送函数的指针
 * @return {*}
 */
unsigned char bluebird_send(Data_t *data_s, void (*uart_send)(unsigned char *, int));

/**
 * 将接收的字节数组解析为数据
 * @param {unsigned char} data_ 接收的字节
 * @param {Data_t} *data_s 数据结构体指针
 * @return {unsigned char} 1:接收成功；0：接收未完成；
 */
unsigned char bluebird_unpack(Data_t *data_s, unsigned char data_);

/**
 * 数据结构体初始化函数
 * @param {Data_t} *data_s 数据结构体
 * @return {*}
 */
void bluebird_init(Data_t *data_s);

#ifdef __cplusplus
}
#endif

#endif
