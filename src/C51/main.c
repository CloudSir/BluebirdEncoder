/*
 * @Author: CloudSir
 * @Github: https://github.com/CloudSir
 * @Date: 2022-12-27 10:10:51
 * @LastEditTime: 2022-12-27 10:19:30
 * @LastEditors: CloudSir
 * @Description: 
 */

#include <stdio.h>

void swap_u16_seq(unsigned char *bytes, int length)
{
    unsigned char tmp;
    for(int i = 0; i < length; i+=2)
    {
        tmp =  bytes[i];
        bytes[i] = bytes[i+1];
        bytes[i+1] = tmp;
    }
}

int main()
{
    union 
    {
        char ch[4];
        unsigned short num[2];
    } data;

    data.num[0] = 0x1234;
    data.num[1] = 0x5678;

    for(int i = 0; i < sizeof(data); i++)
    {
        printf("data.ch[%d] = 0x%2X\n", i, data.ch[i]);
    }

    swap_u16_seq(data.ch, 4);

    printf("\n");


    for(int i = 0; i < sizeof(data); i++)
    {
        printf("data.ch[%d] = 0x%2X\n", i, data.ch[i]);
    }
    
    return 0;
}
