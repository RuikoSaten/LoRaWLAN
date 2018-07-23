#include "packet.h"
#include "String.h"
#include "stdlib.h"


extern LoRaAddress localhost;
extern LoRaAddress defaultHost;

static uint8_t* selectEOT(uint8_t* p,uint8_t dataSize);
static uint8_t* selectSOH(uint8_t* p,uint8_t dataSize);
void destroyData(uint8_t* data);
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
 *  申请一段内存作为数据包的存放  并配置数据包的参数
 *  @param dataBytes 数据段
 *  @param source 源地址
 *  @param  destination 目的地址
 *  @return DataPacket 返回一个数据包指针
 */ 
DataPacket* newPacket_BSD( Byte dataBytes,LoRaAddress source,LoRaAddress destination ){
    DataPacket* packet = malloc(sizeof(DataPacket));
    packet->dataBytes = dataBytes;
    packet->source = source;
    packet->destination = destination;
    packet->count = 0x10;
    return packet;
}


/**
 *  申请一段内存作为数据包的存放  并配置数据包的参数
 *  @param source 源地址
 *  @param  destination 目的地址
 *  @return DataPacket 返回一个数据包指针
 */ 
DataPacket* newPacket_SD(LoRaAddress source,LoRaAddress destination ){
    DataPacket* packet = malloc(sizeof(DataPacket));
    packet->source = source;
    packet->destination = destination;
    packet->count = 0x10;
    return packet;
}


/**
 *  从长度为 dataSize 的字节数组中查找 EOT (0x04)
 *  @param p 字节数组指针
 *  @param dataSize 数组长度
 *  @return 字节指针
 */ 
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


/**
 *  从长度为 dataSize 的字节数组中查找 SOH (0x01)
 *  @param p 字节数组指针
 *  @param dataSize 数组长度
 *  @return 字节指针
 */ 
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
    

} 


/**
 *  负责计算数据报的CRC
 *  @param packet   数据包
 *  @return crc 8位CRC校验码
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
 *  销毁数据包资源
 *  @param packet 数据包指针
 *
 *
 */ 
void destroyPacket(DataPacket* packet){
    if(packet != NULL){
        destroyData(packet->dataBytes.data);
        free(packet);
    }
}


/**
 *  销毁数据指针指向的资源
 *  @param data 数据指针
 *
 *
 */ 
void destroyData(uint8_t* data){
    if(data != NULL){
        free(data);
    }
}

