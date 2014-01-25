//
// creature.h
//

struct creature;

#ifndef CREATURE_H
#define CREATURE_H

#include "item.h"
#include "zone.h"
#include "inventory.h"
#include "form/crtr.h"

#define DEAD (-1)

typedef enum slot {
	NO_SLOT = -1,
	LEFT_HAND = 0,
	RIGHT_HAND,
	HEAD,
	MAX_SLOTS
} slot;

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
	int health, stamina;
	int level;
	int xp, need_xp;
	int attack;
	int ac;
	struct item * slots[MAX_SLOTS];
}creature;

void crtr_init(creature *, cform *);
creature * crtr_new(cform *);
void crtr_free(creature *);

int crtr_move(creature *, int, int);
int crtr_tele(creature *, int, int, struct zone *);
int crtr_attack(creature *, creature *);
int crtr_equip(creature *, struct item *, slot);
void crtr_step(creature *, int);

#endif
