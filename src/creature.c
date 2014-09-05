//
// creature.c
//

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "log.h"
#include "zone.h"
#include "world.h"
#include "config.h"
#include "player.h"
#include "creature.h"
#include "io/display.h"

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

	c->refs = 1;
	c->deceased = 0;
	c->act = NULL;
	c->z_ind = -1;

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
	c->max_stamina = DAYS(3);
	c->health  = c->max_health;
	c->stamina = c->max_stamina;
	c->attack  = 1;
	c->ac      = 1;
	c->sight   = 15;
	c->reflex  = 1;
	c->throw   = 20;
	c->ai	   = 1;
	c->speed   = SEC(1.4);

	c->inv = inv_new(25000);
	for (i = 0; i < MAX_SLOTS; i++) c->slots[i] = NULL;

	trigger_init(c->on_spawn);
	trigger_init(c->on_death);
	trigger_init(c->on_lvlup);
	trigger_init(c->on_act_comp);
	trigger_init(c->on_act_fail);
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

	// There should be no reason for copying a deceased creature
	assert(!p->deceased);

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
	c->throw       = p->throw;
	c->speed       = p->speed;
	c->ai	       = p->ai;
	c->gen_id      = p->gen_id;

	c->on_spawn = p->on_spawn;
	c->on_death = p->on_death;
	c->on_lvlup = p->on_lvlup;
	c->on_act_comp = p->on_act_comp;
	c->on_act_fail = p->on_act_fail;

	return c;
}

//
// Increments the reference count and returns the creature
//
creature * crtr_clone(creature * x)
{
	if (x->refs != NOFREE) {
		x->refs++;
	}
	return x;
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

	info("Spawning %s", crtr_name(c));

	do {
		x = random() % z->width;
		y = random() % z->height;
		assert(timeout--); // FIXME
	} while (!crtr_tele(c, x, y, z));

	trigger_pull(&c->on_spawn, c, NULL);
}

//
// Frees the creature if c->nofree not set
// Will try to remove self from zone
//
void crtr_free(creature * c)
{
	if (c->refs != NOFREE && !--c->refs) {
		assert(c->deceased);

		inv_free(c->inv);
		free(c->generic_name);
		free(c->specific_name);
		free(c->ability);
		free(c);
	}
}

//
// Wraps to crtr_free and pulls the on_death trigger
//
void crtr_death(creature * c, char * meth)
{
	assert(c->z != NULL);

	debug("%s died from %s", crtr_name(c), meth);

	c->deceased = 1;
	trigger_pull(&c->on_death, c, meth);

	zone_rm_crtr(c->z, c);
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

	/*#ifdef SERVER
		zone* old_zone;
		int old_x,old_y;
		old_zone=crtr->z;
		old_x=crtr->x;
		old_y=crtr->y;
	#endif*/
	if (crtr_can_tele(crtr, x, y, z)) {
		z->tiles[x][y].crtr = crtr;

		if (crtr->z != z) {
			// remove from old zone
			if (crtr->z != NULL) {
				zone_rm_crtr(crtr->z, crtr);
			}

			// add to new zone creature list
			if (crtr->z != z) {
				crtr->z_ind = z->crtrs.cnt;
				vector_append(&z->crtrs, crtr);
			}
		} else if (crtr->z != NULL) {
			tileof(crtr)->crtr = NULL;
		}

		// redraw the tiles
		if (PLYR.z != NULL) {
			if (crtr->z == PLYR.z) {
				zone_update(crtr->z, crtr->x, crtr->y);
				wrefresh(dispscr);
			}

			if (z == PLYR.z) {
				zone_update(z, x, y);
				wrefresh(dispscr);
			}
		}

		// update coordinates
		crtr->x = x;
		crtr->y = y;
		crtr->z = z;

		/*#ifdef SERVER
			if(old_zone)
				server_tile_update(&(old_zone->tiles[old_x][old_y]), old_x, old_y);
		#endif*/

		return 1;
	}

	debug("%s failed to teleport to %s@%d,%d", crtr_name(crtr), zone_name(z), x, y);

	return 0;
}

//
// Checks if a creature can move/teleport into a space
//
int crtr_can_tele(creature * crtr, int x, int y, zone * z)
{
	tile * t = zone_at(z, x, y);
	return t != NULL && t->crtr == NULL && !t->impassible;
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
	debug("%s received %d xp", crtr_name(c), xp);
	c->xp += xp;
	if (c->xp > c->need_xp) {
		// level up!
		c->level++;
		c->need_xp = req_xp(c);

		trigger_pull(&c->on_lvlup, c, NULL);
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

	debug("%s attacks %s", crtr_name(attacker), crtr_name(defender));

	damage = random() % (attacker->attack + 1);
	damage -= random() % (defender->ac + 1);
	if (damage < 0) damage = 0;

	if (!(config.god_mode && plyr_is_crtr(defender))) {
		defender->health -= damage;
	}

	if (defender->health <= 0) {
		// death comes to us all
		xp = (defender->level + XP_LEVEL_DIFF) - attacker->level;
		if (xp < 0) xp = 0;

		crtr_xp_up(attacker, xp);
		crtr_death(crtr_clone(defender), "violence");

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
	int x, y, s;
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

		if (!crtr_can_tele(c, c->x+dx, c->y+dy, c->z)) {
			if (dx && crtr_can_tele(c, c->x+dx, 0, c->z)) {
				dy = 0;
			} else if (dy) {
				dx = 0;
			}
		}
		//if(!config.multiplayer)
			crtr_act_aa_move(c, dx, dy);
	}
}

//
// This is called once per game step
//
void crtr_step(creature * c, long steps)
{
	assert(!c->deceased);
	assert(c->health > 0);
	assert(c->stamina > 0);

	// stamina upkeep
	c->stamina -= steps;
	if (c->stamina <= 0) {
		crtr_death(c, "starvation");
		return;
	}

	// handle ai when can perform action
	if (c->act == NULL && !plyr_is_crtr(c)) {
		beast_ai(c);
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
// Tests if a creature can dodge an item
//
int crtr_dodges(creature * c, int difficulty)
{
	int roll;
	if (difficulty < 0) difficulty = 0;
	roll = random() % c->reflex;
	return difficulty < roll || roll == c->reflex;
}

//
// The following functions are typically called when actions are completed
//
void crtr_try_move(creature * c, int dx, int dy)
{
	if (crtr_move(c, dx, dy)) {
		trigger_pull(&c->on_act_comp, c, NULL);
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_MOVE);
	}
}

void crtr_try_aa_move(creature * c, int dx, int dy)
{
	int dam;
	tile * t;
	creature * d;

	//movement in muliplayer is impossible!
	if(config.multiplayer)
		return;

	assert(dx || dy);

	if (!crtr_move(c, dx, dy)) {
		t = zone_at(c->z, c->x + dx, c->y + dy);

		// auto attack
		if (t != NULL && t->crtr != NULL) {
			d = t->crtr;
			assert(c != d);

			switch (dam = crtr_attack(c, d)) {
			case DEAD:
				// TODO memos should probably be in player.c
				if (plyr_is_crtr(c)) memo("You slay the %s.", crtr_name(d));

				crtr_free(d);
				break;
			case 0:
				if (plyr_is_crtr(c)) memo("You miss.");
				if (plyr_is_crtr(d)) memo("You dodge the %s's attack.", crtr_name(c));
				break;
			default:
				if (plyr_is_crtr(c)) memo("You hit for %d damage.", dam);
				if (plyr_is_crtr(d)) memo("You are hit by %s for %d damage.", crtr_name(c), dam);
				break;
			}
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_AA_MOVE);
			return;
		}
	}

	trigger_pull(&c->on_act_comp, c, NULL);
}

void crtr_try_pickup(creature * c, int i)
{
	int j;
	tile * t = tileof(c);

	if (t->inv->size > i && t->inv->itms[i] != NULL) {
		if ((j = inv_add(c->inv, t->inv->itms[i])) != INVALID) {
			t->inv->itms[i]->i = j;
			inv_rm(t->inv, i);

			trigger_pull(&c->on_act_comp, c, c->inv->itms[j]);
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_PICKUP_HEAVY);
		}
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_PICKUP_PRESENT);
	}
}

void crtr_try_drop(creature * c, int i)
{
	int j;
	tile * t = tileof(c);

	if (c->inv->size > i && c->inv->itms[i] != NULL) {
		if ((j = inv_add(t->inv, c->inv->itms[i])) != INVALID) {
			c->inv->itms[i]->i = j;
			inv_rm(c->inv, i);

			trigger_pull(&c->on_act_comp, c, t->inv->itms[j]);
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_DROP_HEAVY);
		}
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_DROP_PRESENT);
	}
}

void crtr_try_consume(creature * c, int i)
{
	item * it;

	if (c->inv->size > i && c->inv->itms[i] != NULL) {
		if (c->inv->itms[i]->type & ITEM_CONSUMABLE) {
			it = crtr_rm_item(c, i);

			c->health  += it->restore_health;
			c->stamina += it->restore_stamina;

			if (c->health  > c->max_health ) c->health  = c->max_health;
			if (c->stamina > c->max_stamina) c->stamina = c->max_stamina;

			trigger_pull(&c->on_act_comp, c, it);
			item_free(it);
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_CONSUME_ABLE);
		}
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_CONSUME_PRESENT);
	}
}

void crtr_try_equip(creature * c, int i)
{
	item * it;

	if (c->inv->size > i && c->inv->itms[i] != NULL) {
		if (c->inv->itms[i]->type & ITEM_EQUIPABLE) {
			it = c->inv->itms[i];
			crtr_equip(c, it, it->slot);

			trigger_pull(&c->on_act_comp, c, it);
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_EQUIP_ABLE);
		}
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_EQUIP_PRESENT);
	}
}

void crtr_try_throw(creature * c, int i, int dx, int dy)
{
	assert(c->inv->size > i);
	assert(c->inv->itms[i] != NULL);

	if (i < c->inv->size && c->inv->itms[i] != NULL) {
		if (item_throw(c->inv->itms[i], c->x, c->y, c->z, dx, dy, c->throw)) {
			crtr_rm_item(c, i);
			trigger_pull(&c->on_act_comp, c, NULL);
			return;
		}
	}

	trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_THROW);
}

//
// The following function schedule creature actions
//
#define ACT_TMPLT(T) \
	assert(c->act == NULL); \
	action * a = malloc(sizeof(action)); \
	c->act = a; \
	a->c = crtr_clone(c); \
	a->type = T

void crtr_act_move(creature * c, int x, int y)
{
	ACT_TMPLT(ACT_MOVE);
	a->p.dir.x = x;
	a->p.dir.y = y;
	schedule(a, c->speed);
}

void crtr_act_aa_move(creature * c, int x, int y)
{
	ACT_TMPLT(ACT_AA_MOVE);
	a->p.dir.x = x;
	a->p.dir.y = y;
	schedule(a, c->speed);
}

void crtr_act_pickup(creature * c, int i)
{
	ACT_TMPLT(ACT_PICKUP);
	a->p.ind = i;
	schedule(a, c->speed);
}

void crtr_act_drop(creature * c, int i)
{
	ACT_TMPLT(ACT_DROP);
	a->p.ind = i;
	schedule(a, c->speed);
}

void crtr_act_consume(creature * c, int i)
{
	ACT_TMPLT(ACT_CONSUME);
	a->p.ind = i;
	schedule(a, c->speed);
}

void crtr_act_equip(creature * c, int i)
{
	ACT_TMPLT(ACT_EQUIP);
	a->p.ind = i;
	schedule(a, c->speed);
}

void crtr_act_throw(creature * c, int i, int x, int y)
{
	ACT_TMPLT(ACT_THROW);
	a->p.throw.ind = i;
	a->p.throw.x   = x;
	a->p.throw.y   = y;
	schedule(a, c->speed);
}

void crtr_act_idle(creature * c)
{
	ACT_TMPLT(ACT_IDLE);
	schedule(a, c->speed);
}
