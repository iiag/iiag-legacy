#ifndef SPELLS_H
#define SPELLS_H

typedef struct spell spell;

// TODO: Define spell structure
struct spell {
	int caliber;

	char * name;

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
