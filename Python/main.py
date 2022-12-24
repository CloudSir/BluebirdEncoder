'''
Author: CloudSir
Github: https://github.com/CloudSir/BluebirdEncoder
Date: 2022-11-10 08:53:45
LastEditTime: 2022-12-24 13:34:01
LastEditors: CloudSir
Description: 青鸟编码器使用示例
'''
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
