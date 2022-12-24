# 青鸟编码器

蓬山此去无多路，青鸟殷勤为探看。——唐·李商隐《无题》

## 简介

![无标题-2022-12-12-1117](_imgs/%E6%97%A0%E6%A0%87%E9%A2%98-2022-12-12-1117-16716877243301.png)

青鸟编码器是一个数据编码/解码的工具库，可用于嵌入式系统间的数据通信：

- 编码：将 int16/uint16 数组编码为数据帧（字节数组）
- 解码：将数据帧（字节数组）解码为 int16/uint16 数组

### Roadmap

- C 语言版本加入初始化函数
- 编写 C51 版本
- 编写 Lua 版本

### 注意事项

- C 语言版本只支持小端序的嵌入式系统，不支持大端序的硬件（如 51单片机）
- C51(大端序) 的版本的青鸟编码器正在开发中
- 如果不知道当前平台是大端序还是小端序，可以使用下列代码进行测试：

```c
/**
 * 或取当前平台的字节序类型
 * @return {int} 1：小端序，0：大端序，-1：获取出错
 */
int get_endianness()
{
    union __Data
    {
        short num;
        char datas[2];
    } Data;

    Data.num = 0x1234;

    if(Data.datas[0] == 0x34)
    {
        // 小端序
        return 1;
    }
    else if(Data.datas[0] == 0x12)
    {
        // 大端序
        return 0;
    }
    else
    {
        // 错误
        return -1;
    }
}
```

## 版本说明

### Python

Python 版本同时兼容 MicroPython 和 Python3.X，支持的部分平台如下：

- MicroPython
    - OpenMV
    - MaixPy(K210)
    - CanMV
    - 树莓派 Pico
    - ESP32 等

- Python3.X
    - 树莓派
    - 香橙派
    - 各种 Linux 开发板 等

### C

C 语言版本兼容各种小端序的嵌入式平台（51 单片机等大端序的硬件请使用 C51 版本），支持的部分平台如下（兼容 C++）：

- STM32
- K210
- Arduino
- 树莓派 Pico
- ESP32
- ESP8266
- MSP430
- MSP432
- 各种 ARM 单片机
- 各种 RISC-V 单片机
- 各种 Linux 开发板 等

### C51

C51 版本兼容 51 单片机等大端序的硬件平台，支持的部分平台如下：

- AT89CXX
- STC89CXX

### Lua

Lua 版本支持 LuatOS 平台。

- Air101
- Air103
- Air105 等支持 LuatOS 的单片机

## 示例

### Python

```python
import bluebird_encoder

send_datas = [-1234, -567, 1456, 32767]

print("原数组：", send_datas)

bluebird = bluebird_encoder.BlueBird()

# 编码
sended_bytes = bluebird.pack(send_datas, is_int16=True)

print("编码后的字节数组:", ",".join([hex(a) for a in sended_bytes]))

# 通过串口发送编码后的字节数组
# uart.send(sended_bytes)  # 方法一
# bluebird.send(uart.send) # 方法二


# 解码
recieved_bytes = [0xeb, 0x90, 0x9, 0x2e, 0xfb, 0xc9, 0xfd, 0xb0, 0x5, 0xff, 0x7f, 0x22, 0xbe]

for char_ in recieved_bytes:
    # 每次只处理一个字节
    # chr 是串口接收到的长度为 1 的字节数组，一般是 uart.read(1) 返回的值
    chr = bytes([char_])
    unpacked_data = bluebird.unpack(chr)
    if unpacked_data: # 如果解码完成
        print("解码后的数组:", unpacked_data)

```

### C

```c
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
```

## 数据帧格式

青鸟编码器的数据帧格式如下，您可以根据该格式自行适配其他语言。

| 位数                           | 意义                             | 说明                                                         |
| ------------------------------ | -------------------------------- | ------------------------------------------------------------ |
| D<sub>0</sub>                  | 帧头                             | 固定值：0xEB                                                 |
| D<sub>1</sub>                  | 帧头                             | 固定值：0x90                                                 |
| D<sub>2</sub>                  | 传输数据的类型;<br />数据长度 N; | d<sub>0</sub>：数据类型(1-int16,0-uint16);<br />d<sub>1</sub>-d<sub>7</sub>：数据的长度 N，范围0-127; |
| D<sub>3</sub>-D<sub>2N+2</sub> | 数据                             | int16 / uint16，小端序（低位在前）                           |
| D<sub>2N+3</sub>               | 校验和                           | 数据位（D<sub>3</sub>-D<sub>2N+2</sub>）相加，和的低8位      |
| D<sub>2N+4</sub>               | 帧尾                             | 固定值：0xBE                                                 |
