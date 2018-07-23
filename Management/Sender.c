#include "Sender.h"
#include "ECC.h" 
//传输层提供一个发送函数
// _Bool LoRaSendData(DataPacket* packet);
// DataPacket 包含了 数据段 源地址 下一跳地址 目的地址 以及CRC校验
// CRC校验不校验下一跳地址 因而不需要每次转发都重新计算


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
//    Byte dataBytes;             //数据段结构体  
//    uint8_t crc;                //源地址+源信道+目的地址+目的信道+数据段 8位crc校验
//    LoRaAddress source;
//    LoRaAddress destination;
//    LoRaAddress nextJmp;
//}DataPacket ;

//LinkedList* newLinkedList(char* listMassage);
//void destroy(LinkedList* list);


// Sender 负责维护一个等待发送的数据列表
// 打包将数据存放到 DataPacket 对象后
// 调用 LoRaSendData 发送函数发送出去
// 发送完成之后负责将数据移交给 Checker 等待校验完成
// 内部需要维护一个最大发送列表长度


LinkedList* sendList = NULL;
_Sender* Sender = NULL;

void send(DataPacket* packet);
void _send(void);
void sendAck(LoRaAddress address,uint8_t ack);
/**
 *  初始化函数
 *  负责申请一段内存存放 sendList 
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
 *  发送函数
 *  负责将数据包添加到待发送列表
 *  确认帧经过发送列表直接发送
 *  @param packet 数据包 内存必须由malloc alloc 申请
 *  
 */ 
void send(DataPacket* packet){
    if(sendList->size >= MAX_LEN){
        sendList->headRemove(sendList);
    }
    sendList->tailInsert(sendList,packet);
}


/**
 *  实际发送函数
 *  由发送任务调用
 *
 *
 */ 
void _send(void){
    if(sendList->size != 0){
        DataPacket* packet = sendList->headRemove(sendList);
        if(LoRaSendData(packet)){
            #if DEBUG
            printf("LORA 发送数据包 crc为 %x\r\n",packet->crc);
            printf("LORA 发送数据包 len为 %x\r\n",packet->dataBytes.length);
            #endif
            if(packet->dataBytes.length > 2){
                //发送完成，如果数据长度大于2，数据包移交给ECC
                ECC->check(packet);
            }else if(packet->dataBytes.length == 1 || packet->dataBytes.length == 2){
                free(packet->dataBytes.data);
                free(packet);
            }
        }
    }
}



/**
 *  发送确认帧
 *  由接收者直接调用
 *
 *
 */ 
void sendAck(LoRaAddress address,uint8_t ack){
    //发送之前先去找到目的地址的下一条地址
    //因为没写好路由表，所以默认下一跳地址是默认地址
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







