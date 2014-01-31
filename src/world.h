//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

#include "util.h"
#include "zone.h"
#include "creature.h"
#include "form/item.h"

typedef struct {//world states
	creature plyr;//player creature
	zone ** zones;//list of zones that exist
	struct vector iforms;
	struct vector cforms;
} world_st;

extern world_st world;//prototype world

void assure_world(void);
void init_world(void);//initializes the world

#endif
