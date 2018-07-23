#include "ECC.h"

_ECC* ECC = NULL;
LinkedList* ECCList = NULL;
void check(DataPacket* packet);
_Bool ECCRemove(uint8_t crc);

/**
 *  ECC单元初始化函数
 *  负责申请内存
 *
 *
 */ 
void ECCInit(void){
    ECCList = newLinkedList("ecc");
    ECC = (_ECC*)malloc(sizeof(_ECC));
    ECC->ECCList = ECCList;
    ECC->check = check;
    ECC->remove = ECCRemove;
}


/**
 *  将一个发送完成的数据包转交给ECC
 *  由发送管理单元调用
 *  @param packet 数据包
 *
 */ 
void check(DataPacket* packet){
    if(ECCList->size >= ECC_MAX_LEN){
        ECCList->headRemove(ECCList);
    }
    ECCList->tailInsert(ECCList,packet);
}


///**
// *  迭代器重写 search 的功能简化 数据比较流程
// *  @param crc 8bit CRC校验码
// *  @return succ or not found
// *
// */ 
//_Bool iterator(uint8_t crc){
//    if(ECCList->size == 0){
//        return 0;
//    }
//    DataPacket* pItem = NULL;
//    Node* pNode = ECCList->first->next;
//    while(pNode->next != NULL){
//        pItem = pNode->nodeData;
//        if(crc == pItem->crc){
//            ECCList->deleteByNode(ECCList,pNode);
//            return 1;
//        }
//    }
//    
//}

/**
 *  从列表中移除校验码为 crc 的节点
 *  由接收管理单元调用
 *  @param crc 要移除的数据包的校验码
 *  @return 成功或失败
 *
 */ 
_Bool ECCRemove(uint8_t crc){
    if(ECCList->size == 0){
        return 0;
    }
    DataPacket* pItem = NULL;
    Node* pNode = NULL;
    Iterator iterator = ECCList->iterator;
    iterator.reset(ECCList);
    while(iterator.hasNext(ECCList)){
        pNode = iterator.next(ECCList);
        pItem = pNode->nodeData;
        if(crc == pItem->crc){
            ECCList->deleteByNode(ECCList,pNode);
            free(pItem->dataBytes.data);//数据空间由 malloc 申请
            free(pItem);                //数据包空间也是由 malloc 申请的
            return 1;
        }
    }
    return 0;
}







