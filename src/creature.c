//
// creature.c
//

#include <math.h>
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

static int req_xp(creature * c)
{
	return (int) (ceil(exp(c->level)) * LEVELING_CONSTANT);
}

void crtr_init(creature * c, cform * f)
{
	int i;

	c->f = form_copy(f);
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

creature * crtr_new(cform * f)
{
	creature * c = malloc(sizeof(creature));
	crtr_init(c, f);
	return c;
}

void crtr_free(creature * c)
{
	if (!c->nofree) {
		cform_free(c->f);
		free(c);
	}
}

static void update2(zone * z, int x1, int y1, int x2, int y2)
{
	if (z != NULL && z == world.plyr.z) {
		zone_update(z, x1, y1);
		zone_update(z, x2, y2);
		wrefresh(dispscr);
	}
}

int crtr_tele(creature * crtr, int x, int y, zone * z)
{
	tile * t = zone_at(z, x, y);

	if (t != NULL && t->crtr == NULL && !t->impassible) {
		z->tiles[x][y].crtr = crtr;

		if (crtr->z != NULL) {
			tileof(crtr)->crtr = NULL;
		}

		update2(crtr->z, x, y, crtr->x, crtr->y);

		crtr->x = x;
		crtr->y = y;
		crtr->z = z;

		return 1;
	}

	return 0;
}

int crtr_move(creature * crtr, int dx, int dy)
{
	int ox = crtr->x;
	int oy = crtr->y;
	int nx = ox + dx;
	int ny = oy + dy;

	return crtr_tele(crtr, nx, ny, crtr->z);
}

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

void crtr_unequip(creature * c, slot sl)
{
	c->attack -= c->slots[sl]->f->modify_attack;
	c->ac     -= c->slots[sl]->f->modify_ac;
	c->slots[sl] = NULL;
}

int crtr_equip(creature * c, item * it, slot sl)
{
	if (it->f->type & ITEM_EQUIPABLE) {
		if (c->slots[sl] != NULL) crtr_unequip(c, sl);

		c->attack += it->f->modify_attack;
		c->ac     += it->f->modify_ac;
		c->slots[sl] = it;

		return 1;
	}

	return 0;
}

int crtr_attack(creature * attacker, creature * defender)
{
	int damage, xp;

	damage = rand() % (attacker->attack + 1);
	damage -= rand() % (defender->ac + 1);
	if (damage < 0) damage = 0;

	defender->health -= damage;

	if (defender->health <= 0) {
		// death comes to us all
		xp = (defender->level + XP_LEVEL_DIFF) - attacker->level;
		if (xp < 0) xp = 0;

		crtr_xp_up(attacker, xp);

		if (plyr_is_me(defender)) {
			plyr_ev_death("violence");
		} else if (!plyr_is_me(attacker)) {
			// the player wants the name of what he killed
			tileof(defender)->crtr = NULL;
			crtr_free(defender);
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

				tileof(c)->crtr = NULL;
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

item * crtr_rm_item(creature * c, int i)
{
	item * it = c->inv->itms[i];

	if (item_equipped(it, c)) {
		crtr_unequip(c, it->f->slot);
	}

	inv_rm(c->inv, i);
	return it;
}
