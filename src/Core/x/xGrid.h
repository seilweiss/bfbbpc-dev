#ifndef XGRID_H
#define XGRID_H

#include <types.h>

struct xGridBound
{
	void* data;
	uint16 gx;
	uint16 gz;
	bool ingrid;
	bool oversize;
	bool deleted;
	bool gpad;
	xGridBound** head;
	xGridBound* next;
};

void xGridBoundInit(xGridBound* gridb, void* data);

#endif