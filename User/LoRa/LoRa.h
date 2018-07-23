#ifndef __LORA_H__
#define __LORA_H__
#include "bsp_usart.h"

#define LoRaUSART USART1


 
#include "packet.h"
extern LoRaAddress localhost;
extern LoRaAddress defaultHost;

 void LoRaSendByte(uint8_t data);
 void LoRaSendBytes(uint8_t length,uint8_t* p);

_Bool LoRaSendData(DataPacket* packet);


void LoRaInit(void);
uint8_t calculateCRC(DataPacket* packet);
#endif /* __LORA_H__ */

