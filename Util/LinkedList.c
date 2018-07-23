#include "LinkedList.h"
#include "String.h"
#include "stdlib.h"

/**
 *  双向链表的通用操作
 *  1.链表的初始化
 *  2.申请一个链表节点
 *  3.链表的头插法
 *  4.链表的尾插法
 *  5.获取链表长度
 *  6.删除链表节点
 *  7.查找指定值的节点
 *  8.销毁链表（释放链表所有节点的内存空间）
 */
Node* newNode(void* nodeData);
void headInsert(LinkedList* list,void* insertData);
void tailInsert(LinkedList* list,void* insertData);
int length(LinkedList* list);
int totalLength(LinkedList* list);
_Bool deleteByData(LinkedList* list,void* nodeData);
Node* search(LinkedList* list,void* nodeData);
Node* searchByIndex(LinkedList* list,int index);
void* popByIndex(LinkedList* list,int index);
_Bool pushByIndex(LinkedList* list,int index,void* data);
void* headRemove(LinkedList* list);
void* tailRemove(LinkedList* list);
_Bool deleteByNode(LinkedList* list,Node* node);
Node* next(LinkedList* list);
_Bool hasNext(LinkedList* list);
void reset(LinkedList* list);
/**
 *  链表的初始化
 *  初始化需要申请首末节点
 *  然后连接首末节点
 *  最后方法指针赋值
 *  传入一个参数标识链表归属
 *  首末节点存储这个归属信息
 */ 
LinkedList* newLinkedList(char* listMassage){
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->first = (Node*)malloc(sizeof(Node));
    list->last = (Node*)malloc(sizeof(Node));
    list->first->nodeData = listMassage;
    list->last->nodeData = listMassage;
    list->first->pre = NULL;
    list->last->next = NULL;
    list->first->next = list->last;
    list->last->pre = list->first;
    list->size=0;
    list->deleteByData = (_Bool (*)(void*,void*))deleteByData;
    list->headInsert = (void (*)(void*,void*))headInsert;
    list->length = (int (*)(void*))length;
    list->search = (Node* (*)(void*,void*))search;
    list->tailInsert = (void (*)(void*,void*))tailInsert;
    list->totalLength = (int (*)(void*))totalLength;
    list->deleteByNode = (_Bool (*)(void*,Node*))deleteByNode;
    list->headRemove = (void*(*)(void*))headRemove;
    list->tailRemove = (void*(*)(void*))tailRemove;
    list->pushByIndex = (_Bool (*)(void*,int,void*))pushByIndex;
    list->popByIndex = (void* (*)(void*,int))popByIndex;
    list->searchByIndex = (Node* (*)(void*,int))searchByIndex;
    list->iterator.present = list->first;
    list->iterator.hasNext = (_Bool (*)(void*))hasNext;
    list->iterator.next = (Node* (*)(void*))next;
    list->iterator.reset = (void (*)(void*))reset;
    return list;
}





/**
 *  2.申请一个链表节点
 *  传入一个void指针赋值到节点数据指针
 *  
 *
 */ 
Node* newNode(void* nodeData){
    Node* pNode = (Node*)malloc(sizeof(Node));
    pNode->nodeData = nodeData;
    pNode->next = NULL;
    pNode->pre = NULL;
    return pNode;
}


/**
 *  3.链表的头插法
 *  将数据从链表头部开始插入，首节点不变，插入到第二节点
 *  @param list 要操作的链表
 *  @param insertData 要插入的数据
 */ 
void headInsert(LinkedList* list,void* insertData){
    Node* pNode = newNode(insertData);
    pNode->next = list->first->next;
    pNode->pre = list->first;
    list->first->next->pre = pNode; //先找到当前的第二个，前驱指针指向申请的节点
    list->first->next = pNode;      //然后找到首节点，将首节点的后继指针指向申请的节点
    list->size++;
}

/**
 *  4.链表的尾插法
 *  将数据从链表尾部开始插入，末节点存储归属信息不变，插入到倒数第二个节点
 *  @param list 要操作的链表
 *  @param insertData 要插入的数据
 */ 
void tailInsert(LinkedList* list,void* insertData){
    Node* pNode = newNode(insertData);
    pNode->pre = list->last->pre;
    pNode->next = list->last;
    list->last->pre->next = pNode; //先找到当前的倒数第二个，后继指针指向申请的节点
    list->last->pre = pNode;      //然后找到末节点，将末节点的前驱指针指向申请的节点
    list->size++;
}


/**
 *  5.获取链表长度
 *  获取除了首末节点外的节点数
 *  @param list 要操作的链表
 *  @return 除了首末节点外的节点数
 */ 
int length(LinkedList* list){
    Node* pTempNode = list->first->next;
    int count = 0;
    while(NULL != pTempNode->next){ //如果有下一个 那么加一
        count++;
        pTempNode = pTempNode->next;
    }
    return count;
}


/**
 *  获取链表实际长度，包含首末节点
 *  @param list 要操作的链表
 *  @return 实际节点数
 *
 */ 
int totalLength(LinkedList* list){
    int len = length(list);
    return len += 2;
}


/**
 *  6.删除链表节点
 *  根据数据指针，如果相同就删除并释放节点内存，数据内存由上一层决定是否释放
 *  从链表首部开始，删除第一个
 *  @param list 要操作的链表
 *  @param nodeData 要比较的数据指针
 */ 
_Bool deleteByData(LinkedList* list,void* nodeData){
    if(list->first->nodeData == nodeData){
        return 0;
    }
    Node* pTempNode = search(list,nodeData);
    if(pTempNode != NULL){
        pTempNode->pre->next = pTempNode->next;
        pTempNode->next->pre = pTempNode->pre;
        free(pTempNode);
        pTempNode = NULL;
        list->size--;
        return 1;
    }
    return 0;
}


/**
 *  7.查找指定值的节点
 *  @param list 要操作的链表
 *  @param nodeData 要比较的数据指针
 *  @return 节点指针
 */ 
Node* search(LinkedList* list,void* nodeData){
   if(list->first->nodeData == nodeData){
        return 0;
    }
    Node* pTempNode = list->first->next;
    while(NULL != pTempNode->next){
        if(pTempNode->nodeData == nodeData){
            return pTempNode;
        }
    }
    return NULL;
}

/**
 *  8.销毁链表（释放链表所有节点的内存空间）
 *  @param list 要操作的链表
 *
 *
 */ 
void destroy(LinkedList* list){
    Node* pTempNode = list->first;
    while(NULL != pTempNode){
        list->first = pTempNode->next;
        free(pTempNode);
        pTempNode = list->first;
    }
    free(list);
}


/**
 *  找到下标为index的节点
 *  @param  list 要操作的链表
 *  @param index 从零开始的下标，相当于数组下标
 *  @return index 节点的指针
 */ 
Node* searchByIndex(LinkedList* list,int index){
     Node* pTempNode;
    if(index <= list->size/2){
        pTempNode = list->first;
        for( ;index >= 0;index--){
            pTempNode = pTempNode->next;
        }
    }else{
        pTempNode = list->last;
        for(index = list->size - index - 1;index >= 0;index--){
            pTempNode = pTempNode->pre;
        }
    }
    return pTempNode;
}



/**
 *  取出下标为 index 的数据
 *  取出后将节点从链表移除 然后销毁节点内存
 *  @param list 要操作的链表
 *  @param index 从零开始的下标，相当于数组下标
 *  @return index 节点的数据指针
 */ 
void* popByIndex(LinkedList* list,int index){
    if(list == NULL || index < 0 || index >= list->size){
        return NULL;
    }
    
    Node* pTempNode = searchByIndex(list,index);
    
    void* nodeData = pTempNode->nodeData;
    pTempNode->pre->next = pTempNode->next;
    pTempNode->next->pre = pTempNode->pre;
    list->size--;
    free(pTempNode);
    return nodeData;
}



/**
 *  将数据插入到下标为 index 的节点中
 *  申请一个节点  这个节点插入到 index 节点处
 *  @param list 要操作的链表
 *  @param index 从零开始的下标，相当于数组下标
 *  @return succ or error
 */ 
_Bool pushByIndex(LinkedList* list,int index,void* data){
    if(list == NULL || index < 0 || index > list->size){
        return 0;
    }
    if(index == 0){
        headInsert(list,data);
    }else if(index == list->size){
        tailInsert(list,data);
    }else{
        Node* node = newNode(data);
        Node* pTempNode = searchByIndex(list,index);
        node->pre = pTempNode->pre;
        node->next = pTempNode;
        pTempNode->pre->next = node;
        pTempNode->pre = node;
        list->size++;
    }
    return 1;
}


/**
 *  弹出第一个数据,相当于堆栈中的pop
 *  @param list 要操作的链表
 *  @return 数据指针
 *
 */ 
void* headRemove(LinkedList* list){
    if(list == NULL || list->size == 0){
        return NULL;
    }
    Node* pTempNode = list->first->next;
    list->first->next = pTempNode->next;
    pTempNode->next->pre = list->first;
    void* data = pTempNode->nodeData;
    free(pTempNode);
    list->size--;
    return data;
}


/**
 *  从链表尾部取出一个数据并释放该节点内存
 *  @param list 要操作的链表
 *  @return 取出的数据指针
 *
 */ 
void* tailRemove(LinkedList* list){
    if(list == NULL || list->size == 0){
        return NULL;
    }
    Node* pTempNode = list->last->pre;
    list->last->pre = pTempNode->pre;
    pTempNode->pre->next = list->last;
    void* data = pTempNode->nodeData;
    free(pTempNode);
    list->size--;
    return data;
}


_Bool deleteByNode(LinkedList* list,Node* node){
    if(node->next != NULL && node->pre != NULL){
        if(node->pre->next == node && node->next->pre == node){
            node->next->pre = node->pre;
            node->pre->next = node->next;
            free(node);
            list->size--;
            return 1;
        }
    }
    return 0;
}


/**
 *  Iterator reset
 *  复位指针
 *  @param list 要操作的列表
 *
 */ 
void reset(LinkedList* list){
    if(list != NULL)
    list->iterator.present = list->first;
}


/**
 *  Iterator hasNext
 *  如果列表内还有下一个就返回true
 *  @param list 要操作的列表
 *
 */ 
_Bool hasNext(LinkedList* list){
    if(list !=  NULL && list->iterator.present != NULL){
        if(list->iterator.present->next != list->last){
            return 1;
        }
    }
    return 0;
}


/**
 *  Iterator next
 *  获取下一个节点并将指针移向下一个节点
 *
 *
 */ 
Node* next(LinkedList* list){
    return list->iterator.present = list->iterator.present->next;
}





