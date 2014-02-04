//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

#include "util.h"
#include "zone.h"
#include "creature.h"
#include "form/item.h"

typedef struct {
	int era, year, month;
	int mday, wday;
	int hour, min;
} world_time_t;

typedef struct {//world states
	creature plyr;//player creature
	cform * plyr_form;
	world_time_t tm;
	struct vector zones;//list of zones that exist
	struct vector iforms;
	struct vector cforms;
	int max_iforms_freq;
	int max_cforms_freq;
} world_st;

extern world_st world;//prototype world

void assure_world(void);
void init_world(void);//initializes the world
void step_world(void);
void get_time(char *, int);

#endif
