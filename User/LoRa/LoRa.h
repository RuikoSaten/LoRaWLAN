#ifndef __LORA_H__
#define __LORA_H__
#include "bsp_usart.h"

#define LoRaUSART USART1

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
    Byte dataBytes;             //���ݶνṹ��  
    uint8_t crc;                //Դ��ַ+Դ�ŵ�+Ŀ�ĵ�ַ+Ŀ���ŵ�+���ݶ� 8λcrcУ��
//    uint8_t sourceAddress_H;    //Դ��ַ�߰�λ
//    uint8_t sourceAddress_L;    //Դ��ַ�Ͱ�λ
//    uint8_t destinationAddress_H;       //Ŀ�ĵ�ַ�߰�λ
//    uint8_t destinationAddress_L;       //Ŀ�ĵ�ַ�Ͱ�λ
//    uint8_t sourceChannel;
//    uint8_t destinationChannel;
//    uint8_t nextJmpAddress_H;
//    uint8_t nextJmpAddress_L;
//    uint8_t nextJmpChannel;
    LoRaAddress source;
    LoRaAddress destination;
    LoRaAddress nextJmp;
    uint8_t count;              //�������� ��ֹ��ѭ��
}DataPacket ;


extern LoRaAddress localhost;
extern LoRaAddress defaultHost;


_Bool CRC_Check(uint8_t* p,uint8_t count);
uint8_t CRC8_Table(uint8_t* p,uint8_t count);
_Bool LoRaSendData(DataPacket* packet);
int8_t unPacket(DataPacket* dataPacket,uint8_t* pdata,uint8_t dataSize);
void receiveCallBack(void);
void LoRaInit(void);
uint8_t calculateCRC(DataPacket* packet);
#endif /* __LORA_H__ */

