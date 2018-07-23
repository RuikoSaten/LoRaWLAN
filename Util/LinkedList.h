#ifndef  LINKEDLIST_H
#define  LINKEDLIST_H


//˫������Ľڵ�
typedef struct _Node{
    void*  nodeData;    //ÿһ���ڵ㶼��һ������ָ��
    struct _Node* pre;  //ÿһ��˫������Ľڵ㶼��ָ��ǰһ���ڵ��ָ��
    struct _Node* next; //ÿһ��˫������Ľڵ㶼��ָ���һ���ڵ��ָ��
}Node;


typedef struct _Iterator{
    Node* present;
    _Bool (*hasNext)(void* list);
    Node* (*next)(void* list);
    void (*reset)(void* list);
}Iterator;


//˫������   �����׽ڵ� ĩ�ڵ� �Լ�ͨ�÷����ĵ���ָ��
typedef struct{
    Node* first; //�׽ڵ�
    Node* last;  //ĩ�ڵ�
    int size;   //������   ������ĩ�ڵ���ĳ���
    Iterator iterator;
    /**
     *  ˫�������ͨ�ò���
     *  //1.����ĳ�ʼ��        
     * // 2.����һ������ڵ�
     *  3.�����ͷ�巨
     *  4.�����β�巨
     *  5.��ȡ������
     *  6.ɾ������ڵ�
     *  7.����ָ��ֵ�Ľڵ�
     *  //8.���������ͷ��������нڵ���ڴ�ռ䣩
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


