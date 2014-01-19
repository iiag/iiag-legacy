//
// creature.h
//

struct creature;

#ifndef CREATURE_H
#define CREATURE_H

#include "zone.h"
#include "inventory.h"
#include "form/crtr.h"

typedef struct creature{
	cform * f;
	char *ability;
	int health;
	int nofree;
	int x, y;
	struct zone * z;
	struct inventory * inv;
}creature;

creature * crtr_new(cform *);
void crtr_free(creature *);

int crtr_move(creature *, int, int);
int crtr_tele(creature *, int, int, struct zone *);

#endif
