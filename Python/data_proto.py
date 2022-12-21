'''
Author: CloudSir
Github: https://github.com/CloudSir
Date: 2022-12-21 21:17:29
LastEditTime: 2022-12-22 06:48:43
LastEditors: CloudSir
Description: 
'''

class Data:

    data_lis = []  # 解码后的数组
    length = 0     # 数据长度
    type = 0       # 数据类型
    check_sum = 0  # 校验和
    head1 = 0xEB   # 帧头
    head2 = 0x90   # 帧头
    tail = 0xBE    # 帧尾

    __state_code = 0
    __i = 0
    __buffer_lis = []
    
    def unpack_data(self, data_byte):
        '''
        data_byte: 长度为1的字节数组（一般是 uart.read(1) 的返回值）
        return {解包后的数据}
        '''        

        # 将传入的字节数组转换为单个字节
        data_byte = data_byte[0]

        if self.__state_code == 0: # 检查帧头
            if self.__i == 0:      
                if data_byte == self.head1: # 第一帧头
                    self.__i +=1
                    return False
                
            elif self.__i == 1:
                self.__i = 0
                if data_byte == self.head2: # 第二帧头
                    self.__state_code += 1
                    return False

            return False

        if self.__state_code == 1:
            self.length = data_byte>>1   # 接收数据的长度
            self.type = data_byte & 0x1  # 接收数据的类型

            self.__state_code += 1

            return False

        if self.__state_code == 2:     # 接收数据
            self.__buffer_lis.append(data_byte) 
            self.__i += 1
            if self.__i >= self.length * 2 :
                self.__state_code += 1

            return False

        if self.__state_code == 3:     # 接收数据校验和
            self.check_sum = data_byte 

            self.__state_code += 1
            
            return False

        if self.__state_code == 4:

            try:
                import struct
            except:
                import ustruct as struct

            self.__state_code = 0
            self.__i = 0
            
            if data_byte == self.tail:  # 检查帧尾
                if sum(self.__buffer_lis)&0xFF == self.check_sum:  # 检查校验和
                    if self.type == 0: # 如果接收uint16
                       tupl =  struct.unpack("H" * self.length, bytes(self.__buffer_lis))
                       self.data_lis = list(tupl)

                       self.__buffer_lis = [] # 清空缓冲数组
                       return self.data_lis
            
                    elif self.type == 1: # 如果接收int16
                       tupl = struct.unpack("h" * self.length, bytes(self.__buffer_lis))
                       self.data_lis = list(tupl)
                       self.__buffer_lis = [] # 清空缓冲数组
                       return self.data_lis
                    
                    return True
        
        return False


    def pack_data(self, data_list, is_int16=False):
        '''
        data_list: 要编码的数组
        is_int16: {True:int16类型；False：uint16类型}
        return {*}
        '''        
        try:
            import struct
        except:
            import ustruct as struct

        send_data = []

        DIC = {
            True: { # int16
                'type': 'h',
                'flag': 0x01,
            },
            False: { # uint16
                'type': 'H',
                'flag': 0x00,
            }
        }

        data_bytes = struct.pack(DIC[is_int16]['type'] * len(data_list), *data_list)

        send_data.append(0xEB)                        # 帧头：0xEB
        send_data.append(0x90)                        # 帧头：0x90
        send_data.append((len(data_list) << 1) 
                            | DIC[is_int16]['flag'])  # 数据长度;数据类型    
        send_data.extend(data_bytes)                  # 数据   
        send_data.append(sum(data_bytes) & 0xFF)      # 校验和
        send_data.append(0xBE)                        # 帧尾：0xBE
        
        return bytes(send_data)
    