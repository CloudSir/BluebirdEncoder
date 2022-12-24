/*
 * @Author: CloudSir
 * @Github: https://github.com/CloudSir/BluebirdEncoder
 * @Date: 2022-11-10 08:53:45
 * @LastEditTime: 2022-12-24 14:47:17
 * @LastEditors: CloudSir
 * @Description: 青鸟编码器使用示例
 */

#include <stdio.h>

#include "bluebird_encoder/bluebird_encoder.h"

/**
 * 串口发送函数，需要根据单片机的类型进行重写
 * @param {uint8_t} *chr 要发送的字节数据
 * @param {int} length 字节数组的长度
 * @return {*}
 */
void uart_send(uint8_t *chr, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("串口发送:0x%02X\n", chr[i]);
    }
}

int main()
{
    Data_t data_packed;
    Data_t data_unpacked;

#if 1 // 将数据编码为字节数组，并通过串口发送
    int16_t s[] = {-1234, -567, 1456, 32767};
    bluebird_pack(&data_packed, s, 4, 1);
    bluebird_send(&data_packed, uart_send);
#endif


#if 1 // 将接收到的字节数组解码为数据
    uint8_t chars[] = {0xEB, 0x90, 0x09, 0x2E, 0xFB, 0xC9, 0xFD, 0xB0, 0x05, 0xFF, 0x7F, 0x22, 0xBE};
    
    for (int i = 0; i < sizeof(chars); i++)
    {
        // 每次只处理一个字节，如果数据解析完成，bluebird_unpack 函数会返回 1
#if 1   //  一般将下列代码放在串口中断中（变量 chr 换成串口接收到的数据）
        char chr = chars[i];
        if (bluebird_unpack(&data_unpacked, chr))
        {
            printf("\n解析数据完成，");

            if (data_unpacked.type == 0) // 接收的类型是uint16
            {
                printf("数据类型是 uint16！\n");
                for(int i = 0; i < data_unpacked.length; i++)
                {
                    printf("data[%d]=%d\n", i, data_unpacked.data_union.data_u16[i]);
                }
            }

            if (data_unpacked.type == 1) // 接收的类型是int16
            {
                printf("数据类型是 int16！\n");
                for(int i = 0; i < data_unpacked.length; i++)
                {
                    printf("data[%d]=%d\n", i, data_unpacked.data_union.data_i16[i]);
                }
            }
        }
#endif

    }
#endif

}
