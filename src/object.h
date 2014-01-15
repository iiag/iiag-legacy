//
// object.h
//

#ifndef OBJECT_H
#define OBJECT_H

#include <ncurses.h>

struct zone;

typedef struct object {
	int x, y;
	struct zone * z;
	chtype ch;
} object;

void obj_move(object *, int, int);
void obj_tele(object *, int, int, struct zone *);

#endif
