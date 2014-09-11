//
// item.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "item.h"
#include "util.h"
#include "config.h"
#include "player.h"
#include "io/display.h"
#include "recipe.h"

//
// Allocates a new item
//
item * item_new(unsigned type, int tile)
{
	item * it = malloc(sizeof(item));

	it->type = type;
	it->tile = tile;
	it->name = NULL;
	it->weight = 1;
	it->spikiness = 0;

	it->iclass = -1;
	it->mat = -1;
	it->quality = -1;

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
	item * it = item_new(pt->type, pt->tile);

	it->name            = copy_str(pt->name);
	it->weight          = pt->weight;
	it->restore_health  = pt->restore_health;
	it->restore_stamina = pt->restore_stamina;
	it->modify_attack   = pt->modify_attack;
	it->modify_ac       = pt->modify_ac;
	it->durability      = pt->durability;
	it->iclass          = pt->iclass;
	it->quality         = pt->quality;
	it->slot            = pt->slot;
	it->spikiness       = pt->spikiness;
	it->mat             = pt->mat;
	it->gen_id          = pt->gen_id;

	return it;
}

//
// Frees an item
//
void item_free(item * it)
{

	free(it->name);
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
int item_throw(item * it, int x, int y, zone * z, int dx, int dy, int force)
{
	creature * c;
	int ret, dam;
	int anim = (z == PLYR.z) && config.throw_anim_delay;
	int timeout = 100 * force / it->weight;
	int tmp = 0;

	x += dx;
	y += dy;

	while (x >= 0 && y >= 0 && x < z->width && y < z->height && !z->tiles[x][y].impassible && timeout) {
		if (x < 0 || x >= z->width) break;
		if (y < 0 || y >= z->height) break;
		if (z->tiles[x][y].impassible) break;

		// handle the animation
		if (anim) {
			if (tmp) {
				z->tiles[x-dx][y-dy].show_tile = tmp;
				zone_draw_tile(z, x-dx, y-dy);
			}

			tmp = z->tiles[x][y].show_tile;
			z->tiles[x][y].show_tile = it->tile;
			zone_draw_tile(z, x, y);
			disp_refresh();

			usleep(1000 * config.throw_anim_delay);
		}

		// creature collision
		c = z->tiles[x][y].crtr;
		if (c != NULL) {
			if (crtr_dodges(c, force / 10)) {
				memo("The %s artfully dodges the %s!", crtr_name(c), it->name);
			} else {
				dam = (it->spikiness + force) / 4 - 4;
				if (dam < 0) dam = 0;
				c->health -= dam;

				if (c->health <= 0) {
					memo("The %s kills the %s!\n", it->name, crtr_name(c));
					crtr_death(c, "projectile impact");
				} else {
					memo("The %s is hit with the %s for %d damage!", crtr_name(c), it->name, dam);
				}

				goto cleanup;
			}
		}

		x += dx;
		y += dy;
		timeout--;

	}

	x -= dx;
	y -= dy;

cleanup:
	ret = item_tele(it, x, y, z);
	zone_update(z, x, y);
	disp_refresh();
	return ret;
}

//
// generates the items name
//
void item_gen_name(item * it){
	if(it->name)
		free(it->name);

	it->name = malloc((it->iclass != -1 ? strlen(((i_class*)item_types.arr[it->iclass])->name): 0)
		 + (it->mat != -1 ? strlen((char*)materials.arr[it->mat]): 0)
		 + (it->quality != -1 ?strlen(quality[it->quality]):0 ) +3);

	it->name[0]=0;

	if(it->quality != -1){
		strcat(it->name, quality[it->quality]);
		strcat(it->name, " ");
	}
	if(it->mat != -1){
		strcat(it->name, (char*)materials.arr[it->mat]);
		strcat(it->name, " ");
	}
	if(it->iclass != -1){
		strcat(it->name, ((i_class*)item_types.arr[it->iclass])->name);
	}

}

