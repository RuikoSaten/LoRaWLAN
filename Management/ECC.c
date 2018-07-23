#include "ECC.h"

_ECC* ECC = NULL;
LinkedList* ECCList = NULL;
void check(DataPacket* packet);
_Bool ECCRemove(uint8_t crc);

/**
 *  ECC��Ԫ��ʼ������
 *  ���������ڴ�
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
 *  ��һ��������ɵ����ݰ�ת����ECC
 *  �ɷ��͹���Ԫ����
 *  @param packet ���ݰ�
 *
 */ 
void check(DataPacket* packet){
    if(ECCList->size >= ECC_MAX_LEN){
        ECCList->headRemove(ECCList);
    }
    ECCList->tailInsert(ECCList,packet);
}


///**
// *  ��������д search �Ĺ��ܼ� ���ݱȽ�����
// *  @param crc 8bit CRCУ����
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
 *  ���б����Ƴ�У����Ϊ crc �Ľڵ�
 *  �ɽ��չ���Ԫ����
 *  @param crc Ҫ�Ƴ������ݰ���У����
 *  @return �ɹ���ʧ��
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
            free(pItem->dataBytes.data);//���ݿռ��� malloc ����
            free(pItem);                //���ݰ��ռ�Ҳ���� malloc �����
            return 1;
        }
    }
    return 0;
}







