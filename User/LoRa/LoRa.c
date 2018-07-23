#include "LoRa.h"
#include "String.h"
#include "stdlib.h"
//void Usart_SendByte( LoRaUSART, uint8_t ch);
//void Usart_SendString( LoRaUSART, char *str);
//void Usart_SendHalfWord( LoRaUSART, uint16_t ch);
//void Usart_SendArray( LoRaUSART, uint8_t *array, uint16_t num);


 void LoRaSendByte(uint8_t data);
 void LoRaSendBytes(uint8_t length,uint8_t* p);
 _Bool CRC_Check(uint8_t* p,uint8_t count);
uint8_t CRC8_Table(uint8_t* p,uint8_t count);
LoRaAddress localhost;
LoRaAddress defaultHost;


void LoRaInit(void){
    
    localhost.Address_H = 0x00;
    localhost.Address_L = 0x02;
    localhost.Channel = 21;
    defaultHost.Address_H = 0x00;
    defaultHost.Address_L = 0x01;
    defaultHost.Channel = 20;
}





/**
 *  在定向模式下，发送数据前需要发送接收方地址、通道共3字节
 *  这三个字节属于数据链路层头部，不需要插入转义字符
 *  @Deprecated
 *
 */ 
void LoRaAddressConfig(uint16_t address,uint8_t channel){
    Usart_SendByte( LoRaUSART, (0xff00&address)>>8);
    Usart_SendByte( LoRaUSART, (0xff&address));
    Usart_SendByte( LoRaUSART, channel);
}



/**
 *  配置 LoRa 定点发送模式下的接收方地址 本质是发送三字节地址通道数据
 *  @param address_H 地址高八位
 *  @param address_L 地址低八位
 *  @param channel    通道号 0-31
 */ 
static void _LoRaAddressConfig(uint8_t address_H,uint8_t address_L,uint8_t channel){
    Usart_SendByte( LoRaUSART, address_H);
    Usart_SendByte( LoRaUSART, address_L);
    Usart_SendByte( LoRaUSART, channel);
}


/**
 *  向 LoRa 发送一个字节数组
 *  @param p 字节数组指针
 *  @param length 字节数组的长度
 *
 */ 
static void LoRaSendBytes(uint8_t length,uint8_t* p){
    for( ; length > 0 ; length-- ){
        switch(*p){
        case 0x01:
        case 0x04:
        case 0x1B:
            Usart_SendByte( LoRaUSART, 0x1B);
        default:
            Usart_SendByte( LoRaUSART, *p);
            break;
        }
        p++;
    }
}




/**
 *  发送数据
 *  传一个 DataPacket* 过来源地址、源信道、目的地址、目的信道
 *  规定上层传下来的数据中前六位要写好
 *
 */ 
_Bool LoRaSendData(DataPacket* packet){
    if (packet == NULL){
        return 0;
    }
    packet->crc = calculateCRC(packet);
    //发送数据之前先发送下一跳地址、信道
    //这里应该去路由表寻找
    _LoRaAddressConfig(defaultHost.Address_H,defaultHost.Address_L,defaultHost.Channel);
    //发送帧起始符
    Usart_SendByte( LoRaUSART, 0x01);
    LoRaSendByte(packet->count);
    //发送源地址、源信道、目的地址、目的信道
    LoRaSendByte(packet->source.Address_H);
    LoRaSendByte(packet->source.Address_L);
    LoRaSendByte(packet->source.Channel);
    LoRaSendByte(packet->destination.Address_H);
    LoRaSendByte(packet->destination.Address_L);
    LoRaSendByte(packet->destination.Channel);
    
    //发送真正要交付到目的主机的数据
    LoRaSendBytes(packet->dataBytes.length,packet->dataBytes.data);
    
    //发送CRC码
    LoRaSendByte(packet->crc);
    
    
    
    
    //发送帧结束符
    Usart_SendByte( LoRaUSART, 0x04);
    
    
    return 1;
}


/**
 *  向 LoRa 发送一个字节
 *  @param data 一字节数据
 *  
 *
 */ 
static void LoRaSendByte(uint8_t data){
    switch(data){
        case 0x01:
        case 0x04:
        case 0x1B:
            Usart_SendByte( LoRaUSART, 0x1B);
        default:
            Usart_SendByte( LoRaUSART, data);
            break;
    }
}
   





   
        








