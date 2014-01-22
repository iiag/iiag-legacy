//
// creature.h
//

struct creature;

#ifndef CREATURE_H
#define CREATURE_H

#include "zone.h"
#include "inventory.h"
#include "form/crtr.h"

#define DEAD (-1)

typedef struct creature{
	// iternalish stuff
	cform * f;
	int nofree;
	int step;

	// position in world
	int x, y;
	struct zone * z;

	// pragmatic fields
	struct inventory * inv;
	char * name;
	char * ability;
	int health;
	int attack;
	int ac;
}creature;

creature * crtr_new(cform *);
void crtr_free(creature *);

int crtr_move(creature *, int, int);
int crtr_tele(creature *, int, int, struct zone *);
int crtr_attack(creature *, creature *);
void crtr_step(creature *, int);

#endif
