#include "Sender.h"
#include "ECC.h" 
//������ṩһ�����ͺ���
// _Bool LoRaSendData(DataPacket* packet);
// DataPacket ������ ���ݶ� Դ��ַ ��һ����ַ Ŀ�ĵ�ַ �Լ�CRCУ��
// CRCУ�鲻У����һ����ַ �������Ҫÿ��ת�������¼���


//typedef struct{
//    uint8_t* data;
//    uint8_t  length;
//}Byte;

//typedef struct{
//    uint8_t Address_H;
//    uint8_t Address_L;
//    uint8_t Channel;
//}LoRaAddress;


//typedef struct{
//    Byte dataBytes;             //���ݶνṹ��  
//    uint8_t crc;                //Դ��ַ+Դ�ŵ�+Ŀ�ĵ�ַ+Ŀ���ŵ�+���ݶ� 8λcrcУ��
//    LoRaAddress source;
//    LoRaAddress destination;
//    LoRaAddress nextJmp;
//}DataPacket ;

//LinkedList* newLinkedList(char* listMassage);
//void destroy(LinkedList* list);


// Sender ����ά��һ���ȴ����͵������б�
// ��������ݴ�ŵ� DataPacket �����
// ���� LoRaSendData ���ͺ������ͳ�ȥ
// �������֮���������ƽ��� Checker �ȴ�У�����
// �ڲ���Ҫά��һ��������б���


LinkedList* sendList = NULL;
_Sender* Sender = NULL;

void send(DataPacket* packet);
void _send(void);
void sendAck(LoRaAddress address,uint8_t ack);
/**
 *  ��ʼ������
 *  ��������һ���ڴ��� sendList 
 *  @param void
 *  @return void
 *  
 */ 
void SenderInit(void){
    sendList = newLinkedList("send");
    Sender = (_Sender*)malloc(sizeof(_Sender));
    if(NULL != Sender){
        Sender->sendList = sendList;
        Sender->send = send;
        Sender->sendAck = sendAck;
    }
}


/**
 *  ���ͺ���
 *  �������ݰ���ӵ��������б�
 *  ȷ��֡���������б�ֱ�ӷ���
 *  @param packet ���ݰ� �ڴ������malloc alloc ����
 *  
 */ 
void send(DataPacket* packet){
    if(sendList->size >= MAX_LEN){
        sendList->headRemove(sendList);
    }
    sendList->tailInsert(sendList,packet);
}


/**
 *  ʵ�ʷ��ͺ���
 *  �ɷ����������
 *
 *
 */ 
void _send(void){
    if(sendList->size != 0){
        DataPacket* packet = sendList->headRemove(sendList);
        if(LoRaSendData(packet)){
            #if DEBUG
            printf("LORA �������ݰ� crcΪ %x\r\n",packet->crc);
            printf("LORA �������ݰ� lenΪ %x\r\n",packet->dataBytes.length);
            #endif
            if(packet->dataBytes.length > 2){
                //������ɣ�������ݳ��ȴ���2�����ݰ��ƽ���ECC
                ECC->check(packet);
            }else if(packet->dataBytes.length == 1 || packet->dataBytes.length == 2){
                free(packet->dataBytes.data);
                free(packet);
            }
        }
    }
}



/**
 *  ����ȷ��֡
 *  �ɽ�����ֱ�ӵ���
 *
 *
 */ 
void sendAck(LoRaAddress address,uint8_t ack){
    //����֮ǰ��ȥ�ҵ�Ŀ�ĵ�ַ����һ����ַ
    //��Ϊûд��·�ɱ�����Ĭ����һ����ַ��Ĭ�ϵ�ַ
    //findNextJmp(address);
    LoRaAddress nextJmp = defaultHost;
    DataPacket* packet = malloc(sizeof(DataPacket));
    packet->dataBytes.data = malloc(sizeof(uint8_t)*1);
    *(packet->dataBytes.data) = ack;
    packet->dataBytes.length = 1;
    packet->destination = address;
    packet->nextJmp = nextJmp;
    packet->source = localhost;
    packet->crc = calculateCRC(packet);
    packet->count = 0x10;
    send(packet);
}







