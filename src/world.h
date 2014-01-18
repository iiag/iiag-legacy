//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

#include "zone.h"
#include "object.h"

typedef struct {//world states
	object plyr;//player object
	zone ** zones;//list of zones that exist
	form ** forms;//list of monster and creatures and generic items that exit
	int form_count;//number of forms
} world_st;

extern int width, height;//protoype variables
extern world_st world;//prototype world

void init_world(void);//initializes the world

#define PLYR world.plyr

#endif
