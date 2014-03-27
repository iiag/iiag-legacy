//
// item.h
//

struct item;

#ifndef ITEM_H
#define ITEM_H

// for item type bit field
#define ITEM_CONSUMABLE 1
#define ITEM_EQUIPABLE  2

#define ITEM_CONSUMABLE_SFT 0
#define ITEM_EQUIPABLE_SFT  1

#include <ncurses.h>
#include "zone.h"
#include "creature.h"
#include "inventory.h"

typedef struct item {
	unsigned type;
	chtype ch;
	char * name;
	int freq;
	int weight;

	// where in what inventory
	struct inventory * of;
	int i;

	// consumable-specific data
	int restore_health;
	int restore_stamina;

	// equipable-specific data
	int modify_attack;
	int modify_ac;
	int slot;
} item;

//
// Creates a new item, to be freed with item_free
//   type, first argument, is a bit field
//
item * item_new(unsigned, chtype);

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
// Teleport an item to a given location
// Sets 'of' and 'i' if successful
// Returns 1 on success, 0 on failure
//
int item_tele(item *, int, int, struct zone *);

//
// Checks if an item is equiped to a creature
//
int item_equipped(item *, struct creature *);

#endif
