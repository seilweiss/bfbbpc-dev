#ifndef ZPICKUPTABLE_H
#define ZPICKUPTABLE_H

#include <types.h>

struct zAssetPickup
{
	uint32 pickupHash;
	uint8 pickupType;
	uint8 pickupIndex;
	uint16 pickupFlags;
	uint32 quantity;
	uint32 modelID;
	uint32 animID;
};

struct zAssetPickupTable
{
	uint32 Magic;
	uint32 Count;
};

void zPickupTableInit();

#endif