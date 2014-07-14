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
void crtr_init(creature * c, chtype ch)
{
	int i;

	c->ch = ch;

	c->nofree = 0;
	c->step = 1;

	c->x = c->y = 0;
	c->z = NULL;

	c->specific_name = NULL;
	c->generic_name = NULL;
	c->ability = NULL;
	c->fctn = NULL;

	c->level = 1;
	c->xp = 0;
	c->need_xp = req_xp(c);

	c->max_health  = 10;
	c->max_stamina = 500;
	c->health  = c->max_health;
	c->stamina = c->max_stamina;
	c->attack  = 1;
	c->ac      = 1;
	c->sight   = 15;
	c->reflex  = 1;
	c->throw   = 20;

	c->inv = inv_new(500);
	for (i = 0; i < MAX_SLOTS; i++) c->slots[i] = NULL;

	trigger_init(c->on_spawn);
	trigger_init(c->on_death);
	trigger_init(c->on_lvlup);
}


//
// Basically allocates and wraps to crtr_init
//
creature * crtr_new(chtype ch)
{
	creature * c = malloc(sizeof(creature));
	crtr_init(c, ch);
	return c;
}

//
// Just copies a creature
//
creature * crtr_copy(const creature * p)
{
	creature * c = crtr_new(p->ch);

	c->specific_name = copy_str(p->specific_name);
	c->generic_name  = copy_str(p->generic_name);
	c->ability       = copy_str(p->ability);
	c->fctn          = p->fctn;

	// TODO inventory

	c->level   = p->level;
	c->xp      = p->xp;
	c->need_xp = p->need_xp;

	c->health      = p->health;
	c->stamina     = p->stamina;
	c->max_health  = p->max_health;
	c->max_stamina = p->max_stamina;
	c->attack      = p->attack;
	c->ac          = p->ac;
	c->sight       = p->sight;

	c->on_spawn = p->on_spawn;
	c->on_death = p->on_death;
	c->on_lvlup = p->on_lvlup;

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

	trigger_pull(&c->on_spawn, c);
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

		trigger_pull(&c->on_lvlup, NULL);
		crtr_xp_up(c, 0);
	}
}

//
// Unequips an item
//
void crtr_unequip(creature * c, slot sl)
{
	if (c->slots[sl] != NULL) {
		c->attack -= c->slots[sl]->modify_attack;
		c->ac     -= c->slots[sl]->modify_ac;
		c->slots[sl] = NULL;
	}
}

//
// Equips an item, unequips whatever was there first
//
int crtr_equip(creature * c, item * it, slot sl)
{
	if (it->type & ITEM_EQUIPABLE) {
		crtr_unequip(c, sl);

		c->attack += it->modify_attack;
		c->ac     += it->modify_ac;
		c->slots[sl] = it;

		return 1;
	}

	return 0;
}

//
// The attacker attacks the defender (attack vs ac)
// Does not free any resources when the defender is killed
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
		trigger_pull(&defender->on_death, "violence");

		return DEAD;
	}

	return damage;
}

//
// Returns the creature name
//  specific>generic>'mysterious creature'
//
const char * crtr_name(const creature * c)
{
	if (c->specific_name != NULL) return c->specific_name;
	if (c->generic_name  != NULL) return c->generic_name;
	return "mysterious creature";
}

//
// As of now, only takes into consideration faction relations
//
int crtr_disposition(const creature * a, const creature * b)
{
	if (a == b) return 1000; // temporary
	return fctn_relto(a->fctn, b->fctn);
}

//
// Controls a creature as a basic beast-like ai
// Attacks creatures based on closeness and disposition, perfering to attack
//   creatures close and of a negative disposition.
//
static void beast_ai(creature * c)
{
	creature * tar;
	int x, y, s, dam, show;
	int dx, dy;
	int tx = 0;
	int ty = 0;
	int score = -1;
	int sight = c->sight + 1;

	int sx = c->x > sight ? c->x - sight : 0;
	int sy = c->y > sight ? c->y - sight : 0;
	int ex = c->x + sight;
	int ey = c->y + sight;

	if (ex > c->z->width)  ex = c->z->width;
	if (ey > c->z->height) ey = c->z->height;

	// find target
	for (x = sx; x < ex; x++) {
		for (y = sy; y < ey; y++) {
			if (zone_can_see(c->z, c->x, c->y, x, y, c->sight)) {
				if (c->z->tiles[x][y].crtr != NULL) {
					s = -crtr_disposition(c, c->z->tiles[x][y].crtr) + 1;
					dx = x - c->x;
					dy = y - c->y;
					s *= c->sight - (int)(sqrt(dx*dx + dy*dy) + 0.5) + 1;

					if (s > score) {
						score = s;
						tx = x;
						ty = y;
					}
				}
			}
		}
	}

	// move towards/attack target (if there is a target)
	if (score > -1) {
		if (c->x > tx) dx = -1;
		else if (c->x < tx) dx = 1;
		else dx = 0;

		if (c->y > ty) dy = -1;
		else if (c->y < ty) dy = 1;
		else dy = 0;

		if (c->x + dx == tx && c->y + dy == ty) {
			tar = c->z->tiles[tx][ty].crtr;
			dam = crtr_attack(c, tar);

			show = tileof(c)->show && tileof(tar)->show;

			switch (dam) {
			case DEAD:
				if (show && (plyr_is_me(c) || plyr_is_me(tar))) memo("%s kills %s", crtr_name(c), crtr_name(tar));
				zone_update(tar->z, tar->x, tar->y);
				wrefresh(dispscr);
				crtr_free(tar);
				break;
			case 0:
				if (show && (plyr_is_me(c) || plyr_is_me(tar))) memo("%s misses %s.", crtr_name(c), crtr_name(tar));
				break;
			default:
				if (show && (plyr_is_me(c) || plyr_is_me(tar))) memo("%s hits %s for %d damage.", crtr_name(c), crtr_name(tar), dam);
			}
		} else {
			if (!crtr_move(c, dx, dy)) {
				if (!dx || !crtr_move(c, dx, 0)) {
					crtr_move(c, 0, dy);
				}
			}
		}
	}
}

//
// This is called once per game step, i.e. once every time for each creature
//   when the player presses a significant key
//
// TODO trigger death event properly
//
void crtr_step(creature * c, int step)
{
	int x, y;
	zone * z;

	if (c->step != step) {
		assert(c->health > 0);
		assert(c->stamina > 0);

		// stamina upkeep
		c->stamina -= 1;
		if (c->stamina <= 0) {
			// FIXME
			if (plyr_is_me(c)) {
				plyr_ev_death("starvation");
			} else {
				memo("%s dies of starvation", crtr_name(c));

				x = c->x;
				y = c->y;
				z = c->z;

				crtr_free(c);
				zone_update(z, x, y);
				wrefresh(dispscr);
			}

			return;
		}

		if (!plyr_is_me(c)) {
			beast_ai(c);
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
		crtr_unequip(c, it->slot);
	}

	inv_rm(c->inv, i);
	return it;
}

//
// Causes a creature to throw an item
// Returns 1 on success, 0 on failure
//
int crtr_throw_item(creature * c, int i, int dx, int dy)
{
	assert(c->inv->size > i);
	assert(c->inv->itms[i] != NULL);

	if (item_throw(c->inv->itms[i], c->x, c->y, c->z, dx, dy, c->throw)) {
		crtr_rm_item(c, i);
		return 1;
	}

	return 0;
}

//
// Tests if a creature can dodge an item
//
int crtr_dodges(creature * c, int difficulty)
{
	int roll;
	if (difficulty < 0) difficulty = 0;
	roll = rand() % c->reflex;
	return difficulty < roll || roll == c->reflex;
}
