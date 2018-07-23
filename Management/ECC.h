#ifndef  ECC_H
#define  ECC_H

#include "Sender.h"
#include "Receiver.h"

#define ECC_MAX_LEN 0x10

typedef struct{
    LinkedList* ECCList;
    void (*check)(DataPacket* packet);
    _Bool (*remove)(uint8_t crc);
}_ECC;

extern _ECC* ECC;
void ECCInit(void);
#endif /* ECC_H */
