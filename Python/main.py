'''
Author: CloudSir
Github: https://github.com/CloudSir
Date: 2022-11-10 08:53:45
LastEditTime: 2022-12-22 06:42:05
LastEditors: CloudSir
Description: 
'''
import data_proto

send_datas = [-1234, -567, 1456, 32767]

print("原数组：", send_datas)

data_cls = data_proto.Data()

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
