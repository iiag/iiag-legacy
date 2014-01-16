//
// object.h
//

#ifndef OBJECT_H
#define OBJECT_H

#include <ncurses.h>

#define FL_NOFREE 1

struct zone;

typedef enum {
	NONE = -1,
	USELESS,
	CREATURE
} obj_type;

typedef struct object {
	obj_type type;
	unsigned flags;
	int x, y, i;
	int weight;
	struct zone * z;
	chtype ch;
} object;

object * obj_new(obj_type, chtype);
void obj_free(object *);
void obj_move(object *, int, int);
void obj_tele(object *, int, int, struct zone *);

#endif
