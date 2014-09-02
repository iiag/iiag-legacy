//
// world.h
//

#ifndef WORLD_H
#define WORLD_H

typedef struct action_node action_node;

#include "item.h"
#include "util.h"
#include "zone.h"
#include "names.h"
#include "creature.h"
#include "generator.h"
#include "room.h"

typedef struct {
	int era, year, month;
	int mday, wday;
	int hour, min, steps;
} world_time_t;

struct action_node {
	long step;
	action * act;
};

typedef struct {
	creature plyr;

	long step;
	world_time_t tm;

	vector_t zones;    // list of zones that exist
	gclass_t * gcrtrs; // generatable creatures
	gclass_t * gitems; // generatable items
	gclass_t * grooms; // generatable rooms 
	gclass_t * gmats;  // generatable materials

	// for the action min-heap
	action_node * acts;
	int acts_cnt;
	int acts_alloc;

	// for name generation
	ethnicity * eth;
} world_t;

// The singleton world object
extern world_t world;

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

//
// Schedule an action
//
void schedule(action *, long);

//
// Removes the next action to be performed from the list
//
action * pop_action(void);

// macros for easy of writing times
#define STEPS_PER_SEC 100
#define SEC(X)   ((X) * STEPS_PER_SEC)
#define MIN(X)   (SEC ((X) * 60))
#define HOUR(X)  (MIN ((X) * 60))
#define DAYS(X)  (HOUR((X) * 24))
#define YEARS(X) (DAYS((X) * 365))

#endif
