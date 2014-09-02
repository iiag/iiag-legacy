//
// inventory.h
//

#ifndef INVENTORY_H
#define INVENTORY_H

typedef struct inventory inventory;

#include "item.h"
#include "creature.h"

#define INFINITE (-1)
#define INVALID  (-1)

struct inventory {
	int size;
	int weight;
	int max_weight;
	item ** itms;
};

//
// Allocates a new, empty inventory
// Takes the max weight (can be INFINITE)
// Should be freed with inv_free
//
inventory * inv_new(int);

//
// Frees an inventory created with inv_new
//
void inv_free(inventory *);

//
// Adds an item to an inventory
// Returns index on success, INVALID on failure
//
int inv_add(inventory *, item *);

//
// Sees if an item will fit in inventory
// Returns 1 on success, 0 on failure
//
int inv_try(inventory *, item *);

//
//count the items in inventory
//
int inv_count(inventory* inv);

//
//empty inventory
//
void inv_clear(inventory * inv);

//
//empty an inventory
//
void inv_empty(inventory* inv);

//
// Removes an item at a given index from an inventory
//
item * inv_rm(inventory *, int);

//
// These 2 functions convert between indicies and character representations of
//   said integers.
//
// Mappings:
//   a-z <-> 0-25
//   A-Z <-> 26-51
//   0-9 <-> 52-61
//
char ind2ch(int);
int  ch2ind(char);

#endif
