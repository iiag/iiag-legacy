//
// item.h
//

struct item;

#ifndef ITEM_H
#define ITEM_H

#include "zone.h"
#include "inventory.h"
#include "form/item.h"

typedef struct item {
	struct iform * f; //form
	struct inventory * of; // inventory item is in
	int i; // index in inventory
} item;

item * item_new(iform *);
void item_free(item *); // does not remove it from its inventory

int item_tele(item *, int, int, struct zone *);

#endif
