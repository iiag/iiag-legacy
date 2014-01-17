//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

#include "zone.h"
#include "object.h"

typedef struct {
	object plyr;
	zone ** zones;
	form ** forms;
	int form_count;
} world_st;

extern int width, height;
extern world_st world;

void init_world(void);

#define PLYR world.plyr

#endif
