//
// object.h
//

#ifndef OBJECT_H
#define OBJECT_H

#include "form.h"

#define FL_NOFREE 1

struct zone;
struct inventory;

typedef struct object {
	unsigned flags;
	int x, y, i;
	struct zone * z;
	form * f;
	struct inventory * inv;
} object;

object * obj_new(form *);
void obj_free(object *);
void obj_move(object *, int, int);
void obj_tele(object *, int, int, struct zone *);

#endif
