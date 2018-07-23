#ifndef  SENDER_H
#define  SENDER_H

#include "String.h"
#include "stdio.h"
#include "LoRa.h"
#include "LinkedList.h"
#include "stdlib.h"

#define MAX_LEN 0x10


#define DEBUG 1


typedef struct{
    LinkedList* sendList;   //Sender ����һ�������б�
    void (*send)(DataPacket* packet);   //����һ��������д�뷢���б�ĺ���
    void (*sendAck)(LoRaAddress address,uint8_t ack);
}_Sender;

extern _Sender* Sender;
void SenderInit(void);
void _send(void);
#endif /* SENDER_H */
