



# 青鸟编码器

## 简介

![无标题-2022-12-12-1117](_imgs/%E6%97%A0%E6%A0%87%E9%A2%98-2022-12-12-1117-16716877243301.png)

青鸟编码器是一个数据编码/解码的工具库，可用于嵌入式系统间的数据通信：

- 编码：将 int16/uint16 数组编码为数据帧（字节数组）
- 解码：将数据帧（字节数组）解码为 int16/uint16 数组

## 数据帧格式

| 位数         | 意义                             | 说明                                                         |
| ------------ | -------------------------------- | ------------------------------------------------------------ |
| D~0~         | 帧头                             | 固定值：0xEB                                                 |
| D~1~         | 帧头                             | 固定值：0x90                                                 |
| D~2~         | 传输数据的类型;<br />数据长度 N; | d~0~：数据类型(1-int16,0-uint16);<br />d~1~-d~7~：数据的长度 N，范围0-127; |
| D~3~-D~2N+2~ | 数据                             | int16 / uint16，小端序（低位在前）                           |
| D~2N+3~      | 校验和                           | 数据位（D~3~-D~2N+2~）相加，和的低8位                        |
| D~2N+4~      | 帧尾                             | 固定值：0xBE                                                 |

## 示例

### Python

```python
import bluebird_encoder

send_datas = [-1234, -567, 1456, 32767]

print("原数组：", send_datas)

data_cls = bluebird_encoder.Data()

# 编码
sended_bytes = data_cls.pack_data(send_datas, is_int16=True)

print("编码后的字节数组:", ",".join([hex(a) for a in sended_bytes]))

# 通过串口发送编码后的字节数组
# uart.send(sended_bytes)


# 解码
recieved_bytes = [0xeb, 0x90, 0x9, 0x2e, 0xfb, 0xc9, 0xfd, 0xb0, 0x5, 0xff, 0x7f, 0x22, 0xbe]

for char_ in recieved_bytes:
    # 每次只处理一个字节
    # chr 是串口接收到的长度为 1 的字节数组，一般是 uart.read(1) 返回的值
    chr = bytes([char_])
    unpacked_data = data_cls.unpack_data(chr)
    if unpacked_data: # 如果解码完成
        print("解码后的数组:", unpacked_data)

```

### C

```c
#include <stdio.h>

#include "bluebird_encoder/bluebird_encoder.h"

/**
 * 串口发送函数，需要根据单片机进行重写
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
    pack_data(&data_packed, s, 4, 1);
    send_data(&data_packed, uart_send);
#endif


#if 1 // 将接收到的字节数组解码为数据
    uint8_t chars[] = {0xEB, 0x90, 0x09, 0x2E, 0xFB, 0xC9, 0xFD, 0xB0, 0x05, 0xFF, 0x7F, 0x22, 0xBE};
    
    for (int i = 0; i < sizeof(chars); i++)
    {
        // 每次只处理一个字节，如果数据解析完成，unpack_data 函数会返回 1
#if 1   //  一般将下列代码放在串口中断中（变量 chr 换成串口接收到的数据）
        char chr = chars[i];
        if (unpack_data(&data_unpacked, chr))
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
```
