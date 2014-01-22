//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

#include "zone.h"
#include "creature.h"
#include "form/item.h"

typedef struct {//world states
	creature plyr;//player creature
	zone ** zones;//list of zones that exist
	iform ** iforms;//list of different types of items that exist
	int iform_cnt;//number of iforms
	cform ** cforms;
	int cform_cnt;
} world_st;

extern world_st world;//prototype world

void init_world(void);//initializes the world

#endif
