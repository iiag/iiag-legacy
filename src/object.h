//
// object.h
//

#ifndef OBJECT_H
#define OBJECT_H

#include <ncurses.h>

struct zone;

typedef enum {
	USELESS,
	CREATURE
} obj_type;

typedef struct object {
	obj_type type;
	int x, y;
	struct zone * z;
	chtype ch;
} object;

object * obj_new(obj_type, chtype);
void obj_free(object *);
void obj_move(object *, int, int);
void obj_tele(object *, int, int, struct zone *);

#endif
