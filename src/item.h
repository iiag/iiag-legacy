//
// item.h
//

#ifndef ITEM_H
#define ITEM_H

typedef struct item item;
typedef struct material material;

// for item type bit field
#define ITEM_CONSUMABLE 1
#define ITEM_EQUIPABLE  2

#define ITEM_CONSUMABLE_SFT 0
#define ITEM_EQUIPABLE_SFT  1

#include "zone.h"
#include "creature.h"
#include "inventory.h"

struct item {
	unsigned type;
	int tile;
	char * name;

	int iclass;
	int mat;
	int quality;
	
	//id for serialization
	int gen_id;

	// where in what inventory
	inventory * of;
	int i;

	int weight; // in 1/100ths of pounds
	int spikiness;
	int durability;

	// consumable-specific data
	int restore_health;
	int restore_stamina;

	// equipable-specific data
	int modify_attack;
	int modify_ac;
	int slot;
};

//
// Creates a new item, to be freed with item_free
//   type, first argument, is a bit field
//
item * item_new(unsigned, int);

//
// Copys an item, typically for instance from prototype creation
//
item * item_copy(const item *);

//
// Frees an item created with item_new
// Does not remove it from it's inventory
//
void item_free(item *);


//
// Generates the item name from its attributes
//
void item_gen_name(item *);

//
// Teleport an item to a given location
// Sets 'of' and 'i' if successful
// Returns 1 on success, 0 on failure
//
int item_tele(item *, int, int, zone *);

//
// Checks if an item is equiped to a creature
//
int item_equipped(item *, creature *);

//
// Throws an item from a location along a path
// Used for both actual throwing and firing projectiles
//
int item_throw(item *, int, int, zone *, int, int, int);

#endif
