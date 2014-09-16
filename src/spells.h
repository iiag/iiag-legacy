#ifndef SPELLS_H
#define SPELLS_H

typedef struct spell spell;

#include "creature.h"

// TODO: Define spell structure
struct spell {
	char * name;
	void (* effect)(spell *, creature *, int, void **);

	int caliber;
	int level;
	int xp;
};

//
// Allocate a new empty spell
//
spell * spell_new(void);

//
// Return a new spell from a combination of two, NULL if incompatible
//
spell * spell_upgrade(spell *, spell *);

void spell_free(spell *);
#endif
