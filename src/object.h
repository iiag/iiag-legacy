//
// object.h
//

#ifndef OBJECT_H
#define OBJECT_H

#include "form.h"
#include "creature.h"
#include "inventory.h"

#define FL_NOFREE 1

struct zone;
struct creature;
struct inventory;

typedef struct object{//generic object
	unsigned flags;//boolean statements
	int x, y, i;//x for x coordinate y for y coordinate and i for index of inventory
	int health;//health of object
	struct zone * z;//for the zone the object is in
	form * f;//the form of the object
	struct inventory * inv;//inventory of the object
}object;

object * obj_new(form *);//pointer to a new object of a specific form pointer
void obj_free(object *);

int crtr_move(struct creature *, int, int);
int crtr_tele(struct creature *, int, int, struct zone *);
int item_move(object *, int, int);
int item_tele(object *, int, int, struct zone *);

#endif
