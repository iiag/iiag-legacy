//
// item.h
//

struct item;

#ifndef ITEM_H
#define ITEM_H

#include "zone.h"
#include "creature.h"
#include "inventory.h"
#include "form/item.h"

typedef struct item {
	struct iform * f; //form
	struct inventory * of; // inventory item is in
	int i; // index in inventory
} item;

//
// Creates a new item, to be freed with item_free
//
item * item_new(iform *);

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
