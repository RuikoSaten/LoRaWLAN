#include "Receiver.h"
#include "LoRa.h"
#include "ECC.h"
/**
 *  接受者的作用主要是负责提取底层串口中断接收到的数据
 *  1.串口中断接收数据会在后面升级为dma+空闲中断
 *  2.串口底层提供一个 receiveEvent 标志位
 *      当标志位置位时相当于接收到一个完整的数据包
 *  3.接收到完整数据包后，Receiver 负责提取数据并交由 LoRa 提供的方法解析
 *  4. LoRa 解析数据需要传入一个 DataPacket 指针 这段内存由malloc申请并移交给LoRa
 *  5. LoRa 会申请一段空间存放数据  这个空间同样由 malloc 申请
 *  6. 由于数据空间由 malloc 申请 实际有效数据传递给应用层 因此数据空间必须由应用层释放
 *  
 */ 
 
Receiver* receiver;
LinkedList* recvList;
DataPacket* receive(void);
/**
 *  临时数据缓冲区
 *
 *
 *
 */ 
uint8_t data[256];

void ReceiverInit(void){
    recvList = newLinkedList("recv");
    receiver = malloc(sizeof(Receiver));
    receiver->recvEvent = 0;
    receiver->RecvList = recvList;
    receiver->receive = receive;
}



/**
 *  该函数是供上层调用获取一个接收到的数据包的
 *  获取完成后将会从接收列表中移除这个数据包
 *  @return DataPacket*   数据包指针
 *
 *
 */ 
DataPacket* receive(void){    
    return recvList->headRemove(recvList);
}



/**
 *  这个函数负责监控串口中断提供的接收完成事件标志位
 *  当串口接收到一帧数据的时候就去获取一帧数据并将数据转移到临时数组data
 *  然后交由 LoRa 提供的 unPacket 方法解包
 *
 */ 
void _receive(void){
    if(receiveEvent){
        memset(data,0,sizeof(data));                //清空临时缓冲区
        int length = preceiveBuffer-receiveBuffer;  //计算帧长度
        preceiveBuffer = receiveBuffer;             //复位串口接收队列指针
        memcpy(data,receiveBuffer,length);          //拷贝数据到临时缓冲区
        memset(receiveBuffer,0,length*sizeof(char));//清空串口接收缓冲区
        DataPacket* packet= malloc(sizeof(DataPacket));
        int8_t flag = unPacket(packet,data,length);
        if(flag == 0 || flag == -1){
            free(packet);
            receiveEvent = 0;
            return ;
        }
        
        if(recvList->size >= RECV_MAX_LEN){
            recvList->headRemove(recvList);
        }
        
        
        if(flag == 1){      //数据是给本机的  不存在跳数计数器为0的问题
          
            #if DEBUG
            printf("LORA 接收到数据包 crc为 %x\r\n",packet->crc);
            printf("LORA 接收到数据包 len为 %x\r\n",packet->dataBytes.length);
            #endif
            
            
            if(packet->dataBytes.length <= 2){
                ECC->remove(packet->dataBytes.data[packet->dataBytes.length-1]);
                free(packet->dataBytes.data);
                free(packet);
                //更新路由表
            }else{
                recvList->tailInsert(recvList,packet);    //将数据加入到接收列表，顶层调用完成的时候free资源
                Sender->sendAck(packet->source,packet->crc);   //直接发送确认包
            }
            
        }else if(flag == 2){//数据不是给本机的
            if(packet->count == 0){
                free(packet->dataBytes.data);
                packet->dataBytes.data = malloc(sizeof(uint8_t)*2);
                packet->dataBytes.length = 2;
                packet->dataBytes.data[0] = 0xff;
                packet->dataBytes.data[1] = packet->crc;
                packet->destination = packet->source;
                packet->source = localhost;
                packet->count = 0x10;
            }
            Sender->send(packet);   
            
        }
        receiveEvent = 0;
    }
    
    
}





// void receiveCallBack(void){
//    
//    if(receiveEvent){
//        memset(data,0,sizeof(data));
//        int length = preceiveBuffer-receiveBuffer;
//        preceiveBuffer = receiveBuffer;
//        
//        memcpy(data,receiveBuffer,length);
//        memset(receiveBuffer,0,length*sizeof(char));
//        DataPacket dataPacket;
//        if(unPacket(&dataPacket,data,length) == 1){
//            printf("源地址：%x\r\n",((0x00ff&dataPacket.source.Address_H)<<8) + dataPacket.source.Address_L);
//            printf("CRC:%x\r\n",dataPacket.crc);
//            Usart_SendArray( LoRaUSART, dataPacket.dataBytes.data,dataPacket.dataBytes.length);
//        }
//        
//        
//        

//        receiveEvent = 0;
//    }
//}



