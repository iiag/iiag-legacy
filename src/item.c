//
// item.c
//

#include <stdlib.h>
#include <unistd.h>
#include "item.h"
#include "util.h"
#include "player.h"
#include "display.h"

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

//
// Throws an item from a position along the given direction
// (x, y, z) + (dx, dy) is the starting position.
// (dx, dy) is the direction the item moves in.
// f is the force of the throw, effects duration and damage of the throw
// Returns 1 on success and 0 on failure of placing the item
//
int item_throw(item * it, int x, int y, zone * z, int dx, int dy, int f)
{
	int ret;
	int anim = (z == PLYR.z);
	chtype tmp = 0;

	x += dx;
	y += dy;

	// TODO timeout from lack of force
	while (x >= 0 && y >= 0 && x < z->width && y < z->height && !z->tiles[x][y].impassible) {

		if (anim) {
			if (tmp) {
				z->tiles[x-dx][y-dy].ch = tmp;
				zone_draw_tile(z, x-dx, y-dy);
			}

			tmp = z->tiles[x][y].ch;
			z->tiles[x][y].ch = it->ch;
			zone_draw_tile(z, x, y);
			wrefresh(dispscr);

			usleep(50000);
		}

		if (z->tiles[x][y].crtr != NULL) {
			memo("The %s is hit with the %s!", crtr_name(z->tiles[x][y].crtr), it->name);
			// TODO damage/xp/force
			goto cleanup;
		}

		x += dx;
		y += dy;
	}

	x -= dx;
	y -= dy;

cleanup:
	ret = item_tele(it, x, y, z);
	zone_update(z, x, y);
	wrefresh(dispscr);
	return ret;
}
