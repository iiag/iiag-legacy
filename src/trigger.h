//
// trigger.h
//

#ifndef TRIGGER_H
#define TRIGGER_H

typedef void (* trigger_cfunc)(void *, void *);

typedef struct trigger {
	int lua_block;
	trigger_cfunc c_func;
} trigger;

//
// Initializes a trigger to trigger nothing
//
#define trigger_init(T) \
	T.lua_block = 0; \
	T.c_func = NULL

//
// Fires the given trigger
//
void trigger_pull(const trigger *, void *, void *);

#endif
