#include "LinkedList.h"
#include "String.h"
#include "stdlib.h"

/**
 *  ˫�������ͨ�ò���
 *  1.����ĳ�ʼ��
 *  2.����һ������ڵ�
 *  3.�����ͷ�巨
 *  4.�����β�巨
 *  5.��ȡ������
 *  6.ɾ������ڵ�
 *  7.����ָ��ֵ�Ľڵ�
 *  8.���������ͷ��������нڵ���ڴ�ռ䣩
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
 *  ����ĳ�ʼ��
 *  ��ʼ����Ҫ������ĩ�ڵ�
 *  Ȼ��������ĩ�ڵ�
 *  ��󷽷�ָ�븳ֵ
 *  ����һ��������ʶ�������
 *  ��ĩ�ڵ�洢���������Ϣ
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
 *  2.����һ������ڵ�
 *  ����һ��voidָ�븳ֵ���ڵ�����ָ��
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
 *  3.�����ͷ�巨
 *  �����ݴ�����ͷ����ʼ���룬�׽ڵ㲻�䣬���뵽�ڶ��ڵ�
 *  @param list Ҫ����������
 *  @param insertData Ҫ���������
 */ 
void headInsert(LinkedList* list,void* insertData){
    Node* pNode = newNode(insertData);
    pNode->next = list->first->next;
    pNode->pre = list->first;
    list->first->next->pre = pNode; //���ҵ���ǰ�ĵڶ�����ǰ��ָ��ָ������Ľڵ�
    list->first->next = pNode;      //Ȼ���ҵ��׽ڵ㣬���׽ڵ�ĺ��ָ��ָ������Ľڵ�
    list->size++;
}

/**
 *  4.�����β�巨
 *  �����ݴ�����β����ʼ���룬ĩ�ڵ�洢������Ϣ���䣬���뵽�����ڶ����ڵ�
 *  @param list Ҫ����������
 *  @param insertData Ҫ���������
 */ 
void tailInsert(LinkedList* list,void* insertData){
    Node* pNode = newNode(insertData);
    pNode->pre = list->last->pre;
    pNode->next = list->last;
    list->last->pre->next = pNode; //���ҵ���ǰ�ĵ����ڶ��������ָ��ָ������Ľڵ�
    list->last->pre = pNode;      //Ȼ���ҵ�ĩ�ڵ㣬��ĩ�ڵ��ǰ��ָ��ָ������Ľڵ�
    list->size++;
}


/**
 *  5.��ȡ������
 *  ��ȡ������ĩ�ڵ���Ľڵ���
 *  @param list Ҫ����������
 *  @return ������ĩ�ڵ���Ľڵ���
 */ 
int length(LinkedList* list){
    Node* pTempNode = list->first->next;
    int count = 0;
    while(NULL != pTempNode->next){ //�������һ�� ��ô��һ
        count++;
        pTempNode = pTempNode->next;
    }
    return count;
}


/**
 *  ��ȡ����ʵ�ʳ��ȣ�������ĩ�ڵ�
 *  @param list Ҫ����������
 *  @return ʵ�ʽڵ���
 *
 */ 
int totalLength(LinkedList* list){
    int len = length(list);
    return len += 2;
}


/**
 *  6.ɾ������ڵ�
 *  ��������ָ�룬�����ͬ��ɾ�����ͷŽڵ��ڴ棬�����ڴ�����һ������Ƿ��ͷ�
 *  �������ײ���ʼ��ɾ����һ��
 *  @param list Ҫ����������
 *  @param nodeData Ҫ�Ƚϵ�����ָ��
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
 *  7.����ָ��ֵ�Ľڵ�
 *  @param list Ҫ����������
 *  @param nodeData Ҫ�Ƚϵ�����ָ��
 *  @return �ڵ�ָ��
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
 *  8.���������ͷ��������нڵ���ڴ�ռ䣩
 *  @param list Ҫ����������
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
 *  �ҵ��±�Ϊindex�Ľڵ�
 *  @param  list Ҫ����������
 *  @param index ���㿪ʼ���±꣬�൱�������±�
 *  @return index �ڵ��ָ��
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
 *  ȡ���±�Ϊ index ������
 *  ȡ���󽫽ڵ�������Ƴ� Ȼ�����ٽڵ��ڴ�
 *  @param list Ҫ����������
 *  @param index ���㿪ʼ���±꣬�൱�������±�
 *  @return index �ڵ������ָ��
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
 *  �����ݲ��뵽�±�Ϊ index �Ľڵ���
 *  ����һ���ڵ�  ����ڵ���뵽 index �ڵ㴦
 *  @param list Ҫ����������
 *  @param index ���㿪ʼ���±꣬�൱�������±�
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
 *  ������һ������,�൱�ڶ�ջ�е�pop
 *  @param list Ҫ����������
 *  @return ����ָ��
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
 *  ������β��ȡ��һ�����ݲ��ͷŸýڵ��ڴ�
 *  @param list Ҫ����������
 *  @return ȡ��������ָ��
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
 *  ��λָ��
 *  @param list Ҫ�������б�
 *
 */ 
void reset(LinkedList* list){
    if(list != NULL)
    list->iterator.present = list->first;
}


/**
 *  Iterator hasNext
 *  ����б��ڻ�����һ���ͷ���true
 *  @param list Ҫ�������б�
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
 *  ��ȡ��һ���ڵ㲢��ָ��������һ���ڵ�
 *
 *
 */ 
Node* next(LinkedList* list){
    return list->iterator.present = list->iterator.present->next;
}





