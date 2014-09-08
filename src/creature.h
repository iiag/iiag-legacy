//
// creature.h
//

#ifndef CREATURE_H
#define CREATURE_H

typedef struct creature creature;

#include <ncurses.h>
#include "item.h"
#include "zone.h"
#include "action.h"
#include "faction.h"
#include "trigger.h"
#include "inventory.h"

// used for return value of attack
#define DEAD (-1)

// special value of 'creature.refs'
#define NOFREE (-1)


// some stance definitions
#define STANCE_NEUTRAL 0
#define STANCE_ATTACK 1
#define STANCE_DEFENSE 2


extern const char * slot_names[];

// enumeration of different equipable slots
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

// The primary creature structure
struct creature {
	chtype ch;

	// iternalish stuff
	int refs;
	int deceased; // creature is dead but references remain
	int step;
	action * act;
	int z_ind; // index in ->z->crtrs

	// position in world
	int x, y;
	zone * z;

	// identification
	char * specific_name;
	char * generic_name;
	char * ability;
	faction * fctn;

	// item fields
	inventory * inv;
	item * slots[MAX_SLOTS];

	// level/xp
	int level;
	int xp, need_xp;

	// skills and stats
	int health, stamina;
	int max_health, max_stamina;
	int attack;
	int ac;
	int sight;
	int reflex;
	int throw;
	int speed;
    int stance;

	//id for serialization
	int gen_id;
	int ai;

	// triggers
	trigger on_spawn;
	trigger on_death;
	trigger on_lvlup;
	trigger on_act_comp;
	trigger on_act_fail;
};

//
// Sets the values of the given creature to the default values
// If called manually (i.e. not through crtr_new) then the creature should
//   probably persist through the program.
//
void crtr_init(creature *, chtype);

//
// Allocates memory for a creature, should be freed with crtr_free
// Returns allocated creature
//
creature * crtr_new(chtype);

//
// Copies a creature, useful for creating creatures from prototype creatures
//
creature * crtr_copy(const creature *);

//
// Increments the reference count, basically
// Does not perform a true copy (deep or shallow)
//
creature * crtr_clone(creature *);

//
// Places the given creature in a given zone
// x, y, and z of creature set
// Triggers on_spawn
//
void crtr_spawn(creature *, zone *);

//
// Frees a creature allocated with crtr_new
//
void crtr_free(creature *);

//
// Marks a creature as dead an frees the creature if necessary
//
void crtr_death(creature *, char *);

//
// Teleports a creature to a given (x, y, z), if possible
// Returns 1 on success, 0 on failure
//
int crtr_tele(creature *, int, int, zone *);

//
// Checks if can teleport to a location
// Returns 1 on success, 0 on failure
//
int crtr_can_tele(creature *, int, int, zone *);

//
// Simply wraps to crtr_tele, but relative to current creature position
// Returns 1 on success, 0 on failure
//
int crtr_move(creature *, int, int);

//
// Gives a creature XP points
//
void crtr_xp_up(creature *, int);

//
// Returns the name of the creature
//
const char * crtr_name(const creature *);

//
// Return the disposition of one creature to another
//
int crtr_disposition(const creature *, const creature *);

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
// Equips an item to the creature (not through an action)
// Returns 1 if successful, 0 if failed
//
int crtr_equip(creature *, item *, slot);

//
// Called once per game step
//
void crtr_step(creature *, long);

//
// Safely removes an item from the creatures inventory
// Do not call inv_rm on the creature's inventory yourself
//
item * crtr_rm_item(creature *, int);

//
// Tests if a creature can dodge something
//
int crtr_dodges(creature *, int);

//
// The following functions are called when an action is completed
// Typically they should only be called as a result of calling crtr_act_*
//
void crtr_try_move(creature *, int, int);
void crtr_try_aa_move(creature *, int, int); // move w/ auto attack
void crtr_try_pickup(creature *, int);
void crtr_try_drop(creature *, int);
void crtr_try_consume(creature *, int);
void crtr_try_equip(creature *, int);
void crtr_try_throw(creature *, int, int, int); // item then dx, dy

//
// The following functions schedule actions to happen
// If an item need be specified, it is by index (of the creature except w/ pickup)
//
void crtr_act_move(creature *, int, int);
void crtr_act_aa_move(creature *, int, int);
void crtr_act_pickup(creature *, int);
void crtr_act_drop(creature *, int);
void crtr_act_consume(creature *, int);
void crtr_act_equip(creature *, int);
void crtr_act_throw(creature *, int, int, int);
void crtr_act_idle(creature * c);

#endif
