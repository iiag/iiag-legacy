//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

#include "zone.h"
#include "object.h"

typedef struct {
	zone ** zones;
	object plyr;
} world_st;

extern int width, height;
extern world_st world;

void init_world(void);

#define PLYR world.plyr

#endif
