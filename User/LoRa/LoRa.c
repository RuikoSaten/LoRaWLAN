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
 *  �ڶ���ģʽ�£���������ǰ��Ҫ���ͽ��շ���ַ��ͨ����3�ֽ�
 *  �������ֽ�����������·��ͷ��������Ҫ����ת���ַ�
 *  @Deprecated
 *
 */ 
void LoRaAddressConfig(uint16_t address,uint8_t channel){
    Usart_SendByte( LoRaUSART, (0xff00&address)>>8);
    Usart_SendByte( LoRaUSART, (0xff&address));
    Usart_SendByte( LoRaUSART, channel);
}



/**
 *  ���� LoRa ���㷢��ģʽ�µĽ��շ���ַ �����Ƿ������ֽڵ�ַͨ������
 *  @param address_H ��ַ�߰�λ
 *  @param address_L ��ַ�Ͱ�λ
 *  @param channel    ͨ���� 0-31
 */ 
static void _LoRaAddressConfig(uint8_t address_H,uint8_t address_L,uint8_t channel){
    Usart_SendByte( LoRaUSART, address_H);
    Usart_SendByte( LoRaUSART, address_L);
    Usart_SendByte( LoRaUSART, channel);
}


/**
 *  �� LoRa ����һ���ֽ�����
 *  @param p �ֽ�����ָ��
 *  @param length �ֽ�����ĳ���
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
 *  ��������
 *  ��һ�� DataPacket* ����Դ��ַ��Դ�ŵ���Ŀ�ĵ�ַ��Ŀ���ŵ�
 *  �涨�ϲ㴫������������ǰ��λҪд��
 *
 */ 
_Bool LoRaSendData(DataPacket* packet){
    if (packet == NULL){
        return 0;
    }
    packet->crc = calculateCRC(packet);
    //��������֮ǰ�ȷ�����һ����ַ���ŵ�
    //����Ӧ��ȥ·�ɱ�Ѱ��
    _LoRaAddressConfig(defaultHost.Address_H,defaultHost.Address_L,defaultHost.Channel);
    //����֡��ʼ��
    Usart_SendByte( LoRaUSART, 0x01);
    LoRaSendByte(packet->count);
    //����Դ��ַ��Դ�ŵ���Ŀ�ĵ�ַ��Ŀ���ŵ�
    LoRaSendByte(packet->source.Address_H);
    LoRaSendByte(packet->source.Address_L);
    LoRaSendByte(packet->source.Channel);
    LoRaSendByte(packet->destination.Address_H);
    LoRaSendByte(packet->destination.Address_L);
    LoRaSendByte(packet->destination.Channel);
    
    //��������Ҫ������Ŀ������������
    LoRaSendBytes(packet->dataBytes.length,packet->dataBytes.data);
    
    //����CRC��
    LoRaSendByte(packet->crc);
    
    
    
    
    //����֡������
    Usart_SendByte( LoRaUSART, 0x04);
    
    
    return 1;
}


/**
 *  �� LoRa ����һ���ֽ�
 *  @param data һ�ֽ�����
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
   





   
        








