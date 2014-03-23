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

extern const char * slot_names[];

typedef enum slot {
	NO_SLOT = -1,
	LEFT_HAND = 0,
	RIGHT_HAND,
	HEAD,
	TORSO,
	CAPE,
	LEGS,
	FEET,
	MAX_SLOTS
} slot;

typedef struct creature {
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
	int sight;
	struct item * slots[MAX_SLOTS];
} creature;

//
// Sets the values of the given creature to the default values
// If called manually (i.e. not through crtr_new) then the creature should
//   probably persist through the program.
//
void crtr_init(creature *, cform *);

//
// Allocates memory for a creature, should be freed with crtr_free
// Returns allocated creature
//
creature * crtr_new(cform *);

//
// Places the given creature in a given zone
// x, y, and z of creature set
// Triggers on_spawn
//
void crtr_spawn(creature *, struct zone *);

//
// Frees a creature allocated with crtr_new
//
void crtr_free(creature *);

//
// Moves the given creature by the given x and y (not to the given x, y)
// Returns 1 on success, 0 on failure
//
int crtr_move(creature *, int, int);

//
// Teleports a creature to a given (x, y, z), if possible
// Returns 1 on success, 0 on failure
//
int crtr_tele(creature *, int, int, struct zone *);

//
// Gives a creature XP points
//
void crtr_xp_up(creature *, int);

//
// Creature A attacks creature B.
// Returns damage or constant DEAD, 0 damage considered miss/blocked
//
int crtr_attack(creature *, creature *);

//
// Unequips an item from the creature
// If slot empty nothing happens
//
void crtr_unequip(creature *, slot);

//
// Equips an item to the creature
// Returns 1 if successful, 0 if failed
//
int crtr_equip(creature *, struct item *, slot);

//
// Called once per game step
//
void crtr_step(creature *, int);

//
// Safely removes an item from the creatures inventory
// Do not call inv_rm on the creature's inventory yourself
//
struct item * crtr_rm_item(creature *, int);

#endif
