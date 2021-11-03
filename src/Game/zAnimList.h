#ifndef ZANIMLIST_H
#define ZANIMLIST_H

#include "xAnim.h"

void zAnimListInit();
void zAnimListExit();
xAnimTable* zAnimListGetTable(uint32 id);
int32 zAnimListGetNumUsed(uint32 id);

#endif