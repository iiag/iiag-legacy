//
// item.c
//

#include <stdlib.h>
#include "item.h"
#include "util.h"

//
// Allocates a new item
//
item * item_new(unsigned type, chtype ch)
{
	item * it = malloc(sizeof(item));

	it->type = type;
	it->ch = ch;
	it->name = NULL;
	it->freq = 1;
	it->weight = 1;

	it->of = NULL;
	it->i = 0;

	it->restore_health = 0;
	it->restore_stamina = 0;

	it->modify_attack = 0;
	it->modify_ac = 0;
	it->slot = 0;

	return it;
}

//
// Copies an item, useful for prototype->instance items
//
item * item_copy(const item * pt)
{
	item * it = item_new(pt->type, pt->ch);

	it->name            = copy_str(pt->name);
	it->freq            = pt->freq;
	it->weight          = pt->weight;
	it->restore_health  = pt->restore_health;
	it->restore_stamina = pt->restore_stamina;
	it->modify_attack   = pt->modify_attack;
	it->modify_ac       = pt->modify_ac;
	it->slot            = pt->slot;

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
	return it->type & ITEM_EQUIPABLE && c->slots[it->slot] == it;
}
