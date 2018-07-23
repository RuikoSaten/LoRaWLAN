#include "LoRa.h"
#include "String.h"
#include "stdlib.h"
//void Usart_SendByte( LoRaUSART, uint8_t ch);
//void Usart_SendString( LoRaUSART, char *str);
//void Usart_SendHalfWord( LoRaUSART, uint16_t ch);
//void Usart_SendArray( LoRaUSART, uint8_t *array, uint16_t num);


static void LoRaSendByte(uint8_t data);
static void LoRaSendBytes(uint8_t length,uint8_t* p);
uint8_t CRCBaton(uint8_t* p,uint8_t count,uint8_t crc8);
static uint8_t* selectEOT(uint8_t* p,uint8_t dataSize);
static uint8_t* selectSOH(uint8_t* p,uint8_t dataSize);
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


//uint8_t data[256];

/**
 *      CRC-8计算表
 *      CRC多项式 10011001
 *      2018年6月10日 16:43:45
 *      siyuan
 */ 
static const char CRC8Table[]={    
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,    
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,    
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,    
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,    
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,    
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,    
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,    
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,    
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,    
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,    
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,    
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,    
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,    
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,    
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,    
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53    
};

/**
 *  传入指定需要校验的字节数组以及长度
 *  如果CRC校验成功就返回1；
 *  否则返回0
 *
 */ 
_Bool CRC_Check(uint8_t* p,uint8_t count){
    uint8_t crc8 = 0;    
    for( ; count > 0; count--){    
        crc8 = CRC8Table[crc8^*p];    
        p++;    
    }
    
    return crc8 == 0?1:0;
}

/**
 *  为了解决计算地址信道CRC问题，将CRC8初始值由00000000改为传入确定
 *
 *
 *
 */     
uint8_t CRCBaton(uint8_t* p,uint8_t count,uint8_t crc8){
    for( ; count > 0; count--){    
        crc8 = CRC8Table[crc8^*p];    
        p++;    
    }    
    return crc8;
} 


/**
 *  传入需要计算CRC的字节数组指针以及长度
 *  返回多项式为10011001的CRC码
 *
 *
 */ 
uint8_t CRC8_Table(uint8_t* p,uint8_t count){    
    unsigned char crc8 = 0;    
    
    for( ; count > 0; count--){    
        crc8 = CRC8Table[crc8^*p];    
        p++;    
    }    
    return crc8;    
    
}

/**
 *  在透明模式下，发送数据前需要发送接收方地址、通道共3字节
 *  这三个字节属于数据链路层头部，不需要插入转义字符
 *  @Deprecated
 *
 */ 
void LoRaAddressConfig(uint16_t address,uint8_t channel){
    Usart_SendByte( LoRaUSART, (0xff00&address)>>8);
    Usart_SendByte( LoRaUSART, (0xff&address));
    Usart_SendByte( LoRaUSART, channel);
}

static void _LoRaAddressConfig(uint8_t address_H,uint8_t address_L,uint8_t channel){
    Usart_SendByte( LoRaUSART, address_H);
    Usart_SendByte( LoRaUSART, address_L);
    Usart_SendByte( LoRaUSART, channel);
}


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
 *  负责计算数据报的CRC
 *
 *
 *
 */ 
uint8_t calculateCRC(DataPacket* packet){
    uint8_t crc8 = 0;
    crc8 = CRCBaton(&packet->source.Address_H,1,crc8);
    crc8 = CRCBaton(&packet->source.Address_L,1,crc8);
    crc8 = CRCBaton(&packet->source.Channel,1,crc8);
    crc8 = CRCBaton(&packet->destination.Address_H,1,crc8);
    crc8 = CRCBaton(&packet->destination.Address_L,1,crc8);
    crc8 = CRCBaton(&packet->destination.Channel,1,crc8);
    crc8 = CRCBaton(packet->dataBytes.data,packet->dataBytes.length,crc8);
    return crc8;
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
   

///**
// *  接收完成回调函数
// *  现已废弃此方法
// *  功能交由receiver负责完成
// *
// */ 
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




/**
 *  数据解析函数
 *  负责解析由receiver传来的数据
 *  @param dataPacket 解析完成后数据存储单元
 *  @param pdata 原始数据指针
 *  @param dataSize 原始数据长度
 *  @return -1  原始数据不符合数据结构
 *  @return 0   数据长度为0
 *  @return 1   数据是给本机的
 *  @return 2   数据不是给本机的 
 *  注意事项： 如果数据不是给本机的话，有两种情况
 *              1. 跳数计数器不为0 那么直接加入待转发列表
 *              2. 跳数计数器为0   那么直接释放资源并返回不可达包
 */ 
int8_t unPacket(DataPacket* dataPacket,uint8_t* pdata,uint8_t dataSize){
    uint8_t *p = pdata,*p1;
    //如果没找到 0x01就直接返回-1
    //如果找到就将pdata移到0x01
    if(NULL == (pdata = selectSOH(pdata,dataSize)))return -1;
    dataSize -= pdata - p;
    //如果没找到0x04就直接返回-1
    if(NULL == (p1 = selectEOT(pdata,dataSize)))return -1;
    //取出CRC
    dataPacket->crc = *(p1-1);
    if(!CRC_Check(pdata+2,p1-pdata-2))return -1;
    
    dataPacket->count = *(pdata+1)-1;
    //取出源地址、信道
    dataPacket->source.Address_H = *(pdata+2);
    dataPacket->source.Address_L = *(pdata+3);
    dataPacket->source.Channel = *(pdata+4);
    //取出目的地址、信道
    dataPacket->destination.Address_H = *(pdata+5);
    dataPacket->destination.Address_L = *(pdata+6);
    dataPacket->destination.Channel = *(pdata+7);
    uint8_t length = p1-pdata-9;
    dataPacket->dataBytes.length = length;
    if(length > 0){
        dataPacket->dataBytes.data = malloc(sizeof(uint8_t)*length);
        memcpy(dataPacket->dataBytes.data,pdata+8,length);
    }else{
        return 0;
    }

    if(dataPacket->destination.Address_H == localhost.Address_H && 
           dataPacket->destination.Address_L == localhost.Address_L &&
           dataPacket->destination.Channel == localhost.Channel ){
        return 1;
    }else{
        return 2;
    }
    
//    else{
//        //如果跳数计数器为0 那么向源主机发送不可达信息
//        
//        if(dataPacket->count == 0){//如果跳数计数器为0 那么向源主机发送不可达信息
//            dataPacket->count = 0x10;
//            dataPacket->destination = dataPacket->source;
//            dataPacket->source = localhost;
//            dataPacket->dataBytes.length = 2;
//            dataPacket->dataBytes.data = malloc(sizeof(uint8_t)*2);
//            dataPacket->dataBytes.data[0] = 0xff;//不可达标志 0xff
//            dataPacket->dataBytes.data[1] = dataPacket->crc;
//            
//        }else{//如果不为0 那么receiver接收完成之后加入到待发送列表
//            uint8_t length = p1-pdata-9;
//            dataPacket->dataBytes.length = length;
//            dataPacket->dataBytes.data = malloc(sizeof(uint8_t)*length);
//            memcpy(dataPacket->dataBytes.data,pdata+8,length);
//        }
//        
//    }
    
//    //如果跳数计数器为0的话 证明是不可达或在不可达的边缘疯狂试探
//    if((dataPacket->count = *(pdata+7)-1) == 0){
//        if(dataPacket->destination.Address_H == localhost.Address_H && 
//           dataPacket->destination.Address_L == localhost.Address_L &&
//           dataPacket->destination.Channel == localhost.Channel ){
//                uint8_t length = p1-pdata-9;
//                dataPacket->dataBytes.length = length;
//               if(length != 0){
//                    dataPacket->dataBytes.data = malloc(sizeof(uint8_t)*length);
//                    memcpy(dataPacket->dataBytes.data,pdata+8,length);
//                   //最后一跳刚好到达了目的地
//                   return 1;
//                }
//        }else{
//            
//        }
//    }
    
    
    
    
    
    
    
    
    
    
//    //查找下一跳地址、信道
//    if(Address_H == localhost.Address_H && 
//       Address_L == localhost.Address_L &&
//       Channel == localhost.Channel ){
//       
//        //目的地址 == 本机地址
//        //解析出数据以及长度
//        uint8_t length = p1-pdata-9;
//        dataPacket->dataBytes.length = length;
//        if(length != 0){
//            dataPacket->dataBytes.data = malloc(sizeof(uint8_t)*length);
//            memcpy(dataPacket->dataBytes.data,pdata+8,length);
//        }
//           
//    }else{
//        //这里应该去查找路由表
//       //但由于路由表算法还没写，所以直接发目的地址
//       //void select(LoRaAddress* pNextJmp);  
//        if(--dataPacket->count >0){//如果跳数不为0 那么直接转发
//            
//            
//            LoRaSendData(dataPacket);
//            
//            
//        }else{//否则应该返回一个不可达包
//            
//        }

//        //找到下一跳地址、信道后直接转发,不需要解析出数据

//       
//        //跳数减一
//        *(pdata+7) = dataPacket->count--;
//        
//        Usart_SendByte( LoRaUSART, 0x01);
//        LoRaSendBytes(p1-pdata-1,pdata+1);
//        Usart_SendByte( LoRaUSART, 0x04);
//        //如果是直接转发就返回0
//           return 0;
//    }
    

}    
        


static uint8_t* selectEOT(uint8_t* p,uint8_t dataSize){
    
    uint8_t* p1 = p + dataSize - 1;
    while(dataSize > 0){
        if(*p1 == 0x04){
            return p1;
        }
        p1--;
        dataSize--;
    }
    return NULL;
}



static uint8_t* selectSOH(uint8_t* p,uint8_t dataSize){
    uint8_t* p1 = p;
    while(dataSize > 0){
        if(*p1 == 0x01){
            return p1;
        }
        p1++;
        dataSize--;
    }
    return NULL;
    
}





