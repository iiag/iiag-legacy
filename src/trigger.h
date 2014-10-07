//
// trigger.h
//

#ifndef TRIGGER_H
#define TRIGGER_H

#include "sol/sol.h"

struct creature;
typedef struct creature creature;

typedef void (* trigger_cfunc)(creature *, void *);

typedef struct trigger {
	sol_object_t *sol_func;
	trigger_cfunc c_func;
} trigger;

//
// Initializes a trigger to trigger nothing
//
#define trigger_init(T) \
	T.sol_func = NULL; \
	T.c_func = NULL

//
// Fires the given trigger
//
void trigger_pull(const trigger *, creature *, void *);

#endif
