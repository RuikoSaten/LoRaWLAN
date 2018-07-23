#ifndef  PACKET_H
#define  PACKET_H

#include "bsp_usart.h"

typedef struct{
    uint8_t* data;
    uint8_t  length;
}Byte;

typedef struct{
    uint8_t Address_H;
    uint8_t Address_L;
    uint8_t Channel;
}LoRaAddress;


typedef struct{
    Byte dataBytes;             //数据段结构体  
    uint8_t crc;                //源地址+源信道+目的地址+目的信道+数据段 8位crc校验
    LoRaAddress source;
    LoRaAddress destination;
    LoRaAddress nextJmp;
    uint8_t count;              //跳数倒数 防止死循环
}DataPacket;

int8_t unPacket(DataPacket* dataPacket,uint8_t* pdata,uint8_t dataSize);
DataPacket* newPacket_BSD( Byte dataBytes,LoRaAddress source,LoRaAddress destination );
DataPacket* newPacket_SD(LoRaAddress source,LoRaAddress destination );
uint8_t CRCBaton(uint8_t* p,uint8_t count,uint8_t crc8);
_Bool CRC_Check(uint8_t* p,uint8_t count);
uint8_t CRC8_Table(uint8_t* p,uint8_t count);
uint8_t calculateCRC(DataPacket* packet);
void destroyPacket(DataPacket* packet);
#endif /* PACKET_H */


