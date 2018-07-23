#ifndef  RECEIVER_H
#define  RECEIVER_H

#include "ECC.h"

#define RECV_MAX_LEN 0x10

typedef struct _Receiver{
    _Bool recvEvent;
    LinkedList* RecvList;
    DataPacket* (*receive)(void);
}Receiver;

extern Receiver* receiver;

void ReceiverInit(void);
void _receive(void);
//DataPacket* receive(void);
#endif /* RECEIVER_H */

