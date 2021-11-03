#include "xGrid.h"

UNCHECKED void xGridBoundInit(xGridBound* gridb, void* data)
{
    gridb->data = data;
    gridb->gx = 0xffff;
    gridb->gz = 0xffff;
    gridb->ingrid = false;
    gridb->oversize = false;
    gridb->head = NULL;
    gridb->next = NULL;
    gridb->gpad = 0xea;
}