//
// action.h
//

#ifndef ACTION_H
#define ACTION_H

typedef struct action action;

#include "creature.h"

// identifies the type of action to take
typedef enum action_type {
	ACT_IDLE,
	ACT_MOVE,
	ACT_AA_MOVE, // move w/ auto attack
	ACT_PICKUP,
	ACT_DROP,
	ACT_CONSUME,
	ACT_EQUIP,
	ACT_THROW,
	ACT_USE,     // use tile object
	ACT_CAST,
} action_type;

// Identifies how an action failed
// Of type void* so can be easily passed to triggers
typedef enum action_fail {
	ACT_FAIL_MOVE,
	ACT_FAIL_AA_MOVE,
	ACT_FAIL_PICKUP_HEAVY,
	ACT_FAIL_PICKUP_PRESENT,
	ACT_FAIL_DROP_HEAVY,
	ACT_FAIL_DROP_PRESENT,
	ACT_FAIL_CONSUME_ABLE,
	ACT_FAIL_CONSUME_PRESENT,
	ACT_FAIL_EQUIP_ABLE,
	ACT_FAIL_EQUIP_PRESENT,
	ACT_FAIL_THROW,
	ACT_FAIL_USE,
	ACT_FAIL_CAST,
} action_fail;

#define V_ACT_FAIL_MOVE            ((void *) ACT_FAIL_MOVE)
#define V_ACT_FAIL_AA_MOVE         ((void *) ACT_FAIL_AA_MOVE)
#define V_ACT_FAIL_PICKUP_HEAVY    ((void *) ACT_FAIL_PICKUP_HEAVY)
#define V_ACT_FAIL_PICKUP_PRESENT  ((void *) ACT_FAIL_PICKUP_PRESENT)
#define V_ACT_FAIL_DROP_HEAVY      ((void *) ACT_FAIL_DROP_HEAVY)
#define V_ACT_FAIL_DROP_PRESENT    ((void *) ACT_FAIL_DROP_PRESENT)
#define V_ACT_FAIL_CONSUME_ABLE    ((void *) ACT_FAIL_CONSUME_ABLE)
#define V_ACT_FAIL_CONSUME_PRESENT ((void *) ACT_FAIL_CONSUME_PRESENT)
#define V_ACT_FAIL_EQUIP_ABLE      ((void *) ACT_FAIL_EQUIP_ABLE)
#define V_ACT_FAIL_EQUIP_PRESENT   ((void *) ACT_FAIL_EQUIP_PRESENT)
#define V_ACT_FAIL_THROW           ((void *) ACT_FAIL_THROW)
#define V_ACT_FAIL_USE             ((void *) ACT_FAIL_USE)
#define V_ACT_FAIL_CAST            ((void *) ACT_FAIL_CAST)


// structure containing information on an action to perform
struct action {
	action_type type;
	creature * c;

	// parameters to the action
	union {
		struct {
			int x, y;
		} dir;
		struct {
			int ind, x, y;
		} throw;
		struct {
			int ind, x, y;
		} cast;
		int ind;
	} p;
};

#endif
