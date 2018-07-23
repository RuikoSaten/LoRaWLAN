#ifndef  LINKEDLIST_H
#define  LINKEDLIST_H


//双向链表的节点
typedef struct _Node{
    void*  nodeData;    //每一个节点都有一个数据指针
    struct _Node* pre;  //每一个双向链表的节点都有指向前一个节点的指针
    struct _Node* next; //每一个双向链表的节点都有指向后一个节点的指针
}Node;


typedef struct _Iterator{
    Node* present;
    _Bool (*hasNext)(void* list);
    Node* (*next)(void* list);
    void (*reset)(void* list);
}Iterator;


//双向链表   包含首节点 末节点 以及通用方法的调用指针
typedef struct{
    Node* first; //首节点
    Node* last;  //末节点
    int size;   //链表长度   除了首末节点外的长度
    Iterator iterator;
    /**
     *  双向链表的通用操作
     *  //1.链表的初始化        
     * // 2.申请一个链表节点
     *  3.链表的头插法
     *  4.链表的尾插法
     *  5.获取链表长度
     *  6.删除链表节点
     *  7.查找指定值的节点
     *  //8.销毁链表（释放链表所有节点的内存空间）
     */
    void (*headInsert)(void* this,void* insertData);
    void (*tailInsert)(void* this,void* insertData);
    int (*length)(void* this);
    int (*totalLength)(void* this);
    _Bool (*deleteByData)(void* this,void* nodeData);
    Node* (*search)(void* this,void* nodeData);
    void* (*headRemove)(void* this);
    void* (*tailRemove)(void* this);
    _Bool (*pushByIndex)(void* this,int index,void* data);
    void* (*popByIndex)(void* this,int index);
    Node* (*searchByIndex)(void* this,int index);
    _Bool (*deleteByNode)(void*,Node* node);
}LinkedList;


LinkedList* newLinkedList(char* listMassage);
void destroy(LinkedList* list);
#endif /* LINKEDLIST_H */


