//
// item.c
//

#include <stdlib.h>
#include "item.h"

//
// Allocates a new item
//
item * item_new(iform * f)
{
	item * it = malloc(sizeof(item));
	it->of = NULL;
	it->i = 0;
	it->f = form_assign(f);
	return it;
}

//
// Frees an item
//
void item_free(item * it)
{
	free(it);
}

//
// Teleports an item
// Fails if cannot add to the destination tile inventory
//
int item_tele(item * it, int x, int y, zone * z)
{
	int i = inv_add(z->tiles[x][y].inv, it);

	if (i != INVALID) {
		if (it->of != NULL) {
			inv_rm(it->of, it->i);
		}

		it->of = z->tiles[x][y].inv;
		it->i = i;

		return 1;
	}

	return 0;
}

//
// Checks if the item is equipped to a creature
//
int item_equipped(item * it, creature * c)
{
	return it->f->type & ITEM_EQUIPABLE && c->slots[it->f->slot] == it;
}
