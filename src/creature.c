//
// creature.c
//

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "log.h"
#include "zone.h"
#include "world.h"
#include "player.h"
#include "display.h"
#include "creature.h"

// lower => levelup faster
#define LEVELING_CONSTANT 4

// player_level - XP_LEVEL_DIFF = minimum creature level for gaining xp
#define XP_LEVEL_DIFF 5

// iterations before spawn attempts timeout
#define SPAWN_TIMEOUT 5000

//
// For a given creature, calculates the required XP to level up
//
static int req_xp(creature * c)
{
	return (int) (ceil(exp(c->level)) * LEVELING_CONSTANT);
}

//
// Initializes the creature, all values are set
// Calls form_assign for f
//
void crtr_init(creature * c, cform * f)
{
	int i;

	c->f = form_assign(f);
	c->nofree = 0;
	c->step = 1;

	c->x = c->y = 0;
	c->z = NULL;

	c->health  = f->max_health;
	c->stamina = f->max_stamina;
	c->ability = NULL;
	c->attack  = f->def_attack;
	c->ac      = f->def_ac;
	c->name    = NULL;
	c->level   = 1;
	c->need_xp = req_xp(c);
	c->xp      = 0;

	for (i = 0; i < MAX_SLOTS; i++) c->slots[i] = NULL;
}

//
// Basically allocates and wraps to crtr_init
//
creature * crtr_new(cform * f)
{
	creature * c = malloc(sizeof(creature));
	crtr_init(c, f);
	return c;
}

//
// Places the creature at a random location in the zone
// crtr_tele handles placing the creature
//
// TODO fix what happens on timeout
//
void crtr_spawn(creature * c, zone * z)
{
	int timeout = SPAWN_TIMEOUT;
	int x, y;

	do {
		x = random() % z->width;
		y = random() % z->height;
		assert(timeout--); // FIXME
	} while (!crtr_tele(c, x, y, z));

	trigger_pull(c->f->on_spawn, c);
}

//
// Frees the creature if c->nofree not set
// Will try to remove self from zone
//
void crtr_free(creature * c)
{
	if (!c->nofree) {
		if (c->z != NULL) {
			assert(tileof(c)->crtr == c);
			tileof(c)->crtr = NULL;
		}

		cform_free(c->f);
		free(c);
	}
}

//
// This takes a creature, and tries to place it somewhere
//
// Will fail if target square is:
//   already occupied
//   impassible
//   nonexistant
//
int crtr_tele(creature * crtr, int x, int y, zone * z)
{
	tile * t = zone_at(z, x, y);

	if (t != NULL && t->crtr == NULL && !t->impassible) {
		z->tiles[x][y].crtr = crtr;

		if (crtr->z != NULL) {
			tileof(crtr)->crtr = NULL;

			// redraw the tiles
			if (crtr->z == world.plyr.z) {
				zone_update(z, x, y);
				zone_update(z, crtr->x, crtr->y);
				wrefresh(dispscr);
			}
		}

		crtr->x = x;
		crtr->y = y;
		crtr->z = z;

		return 1;
	}

	return 0;
}

//
// Simply wraps to crtr_tele, but relative
//
int crtr_move(creature * crtr, int dx, int dy)
{
	int nx = crtr->x + dx;
	int ny = crtr->y + dy;

	return crtr_tele(crtr, nx, ny, crtr->z);
}

//
// Gives a creature expirence points an
//
void crtr_xp_up(creature * c, int xp)
{
	c->xp += xp;
	if (c->xp > c->need_xp) {
		// level up!
		c->level++;
		c->need_xp = req_xp(c);

		if (plyr_is_me(c)) plyr_ev_lvlup();
		crtr_xp_up(c, 0);
	}
}

//
// Unequips an item
//
void crtr_unequip(creature * c, slot sl)
{
	if (c->slots[sl] != NULL) {
		c->attack -= c->slots[sl]->f->modify_attack;
		c->ac     -= c->slots[sl]->f->modify_ac;
		c->slots[sl] = NULL;
	}
}

//
// Equips an item, unequips whatever was there first
//
int crtr_equip(creature * c, item * it, slot sl)
{
	if (it->f->type & ITEM_EQUIPABLE) {
		crtr_unequip(c, sl);

		c->attack += it->f->modify_attack;
		c->ac     += it->f->modify_ac;
		c->slots[sl] = it;

		return 1;
	}

	return 0;
}

//
// The attacker attacks the defender (attack vs ac)
// Does not free any resources when the defender is killed
//
// TODO trigger death event (don't directly call plyr_ev_death)
//
int crtr_attack(creature * attacker, creature * defender)
{
	int damage, xp;

	damage = random() % (attacker->attack + 1);
	damage -= random() % (defender->ac + 1);
	if (damage < 0) damage = 0;

	defender->health -= damage;

	if (defender->health <= 0) {
		// death comes to us all
		xp = (defender->level + XP_LEVEL_DIFF) - attacker->level;
		if (xp < 0) xp = 0;

		crtr_xp_up(attacker, xp);

		if (plyr_is_me(defender)) {
			plyr_ev_death("violence");
		}

		return DEAD;
	}

	return damage;
}

void crtr_step(creature * c, int step)
{
	int dam;
	int dx = 0, dy = 0;
	int x, y;
	zone * z;

	if (c->step != step) {
		assert(c->health > 0);
		assert(c->stamina > 0);

		// stamina upkeep
		c->stamina -= 1;
		if (c->stamina <= 0) {
			if (plyr_is_me(c)) {
				plyr_ev_death("starvation");
			} else {
				memo("%s dies of starvation", c->f->name);

				x = c->x;
				y = c->y;
				z = c->z;

				crtr_free(c);
				zone_update(z, x, y);
				wrefresh(dispscr);
			}

			return;
		}

		// ai section
		if (!plyr_is_me(c)) {
			// this is pretty ugly

			if (c->x > world.plyr.x) dx = -1;
			else if (c->x < world.plyr.x) dx = 1;

			if (c->y > world.plyr.y) dy = -1;
			else if (c->y < world.plyr.y) dy = 1;

			if (PLYR.x == c->x + dx && PLYR.y == c->y + dy) {
				dam = crtr_attack(c, &PLYR);

				if (dam) {
					memo("%s hits you for %d damage", c->f->name, dam);
				} else {
					memo("%s misses you", c->f->name);
				}
			} else {
				if (!crtr_move(c, dx, dy) && dx && dy) {
					if (!crtr_move(c, dx, 0)) {
						crtr_move(c, 0, dy);
					}
				}
			}
		}

		c->step = step;
	}
}

//
// If equipped, unequip item, then remove from inventory
//
item * crtr_rm_item(creature * c, int i)
{
	item * it = c->inv->itms[i];

	if (item_equipped(it, c)) {
		crtr_unequip(c, it->f->slot);
	}

	inv_rm(c->inv, i);
	return it;
}
