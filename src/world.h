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

// The singleton world object
extern world_st world;

//
// Assures that the world is in a manipulatable state
// Should be called if you don't know if init_world has be called yet
//
void assure_world(void);

//
// Initializes the world
//
void init_world(void);

//
// Performs a single game step over the entire world
//
void step_world(void);

//
// Gets a string representing the current time of the world
//
void get_time(char *, int);

#endif
