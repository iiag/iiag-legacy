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

///
/// For a given creature, calculates the required XP to level up.
///
/// \param c The operand creature.
/// \return The calculated amount of XP.
///
static int req_xp(creature * c)
{
	return (int) (ceil(exp(c->level)) * LEVELING_CONSTANT);
}

///
/// Initializes the creature, all values are set.
///
/// \param c The creature to initialize.
/// \param tile The tilenum to use for the creature.
///
void crtr_init(creature * c, int tile)
{
	int i;

	c->tile = tile;

	c->refs = 1;
	c->deceased = 0;
	c->act = NULL;
	c->z_ind = -1;

	c->x = c->y = 0;
	c->z = NULL;

	c->specific_name = NULL;
	c->generic_name = NULL;
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
	c->stance  = STANCE_NEUTRAL;
	c->speed   = SEC(1.4);

	c->inv = inv_new(25000);
	for (i = 0; i < MAX_SLOTS; i++) c->slots[i] = NULL;

	c->lib = lib_new(1000); // TODO: Set a good default intelligence value?

	trigger_init(c->on_spawn);
	trigger_init(c->on_death);
	trigger_init(c->on_lvlup);
	trigger_init(c->on_act_comp);
	trigger_init(c->on_act_fail);
}

///
/// Basically allocates and wraps to crtr_init
///
/// \newref{crtr_free}
///
/// \return A new creature.
///
creature * crtr_new(int tile)
{
	creature * c = malloc(sizeof(creature));
	crtr_init(c, tile);
	return c;
}

///
/// Just copies a creature
///
/// \note
/// At present, this does not work with deceased creatures.
///
/// \newref{crtr_free}
///
/// \param p The creature to copy.
/// \return A new creature.
///
creature * crtr_copy(const creature * p)
{
	creature * c = crtr_new(p->tile);

	// There should be no reason for copying a deceased creature
	assert(!p->deceased);

	c->specific_name = copy_str(p->specific_name);
	c->generic_name  = copy_str(p->generic_name);
	c->fctn          = p->fctn;

	// TODO: inventory
	// TODO: library

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

///
/// Increments the reference count and returns the creature
///
/// \note
/// This function returns the parameter verbatim.
///
/// \param x The operand creature.
/// \return x.
///
creature * crtr_clone(creature * x)
{
	if (x->refs != NOFREE) {
		x->refs++;
	}
	return x;
}

///
/// Places the creature at a random location in the zone
///
/// \note
/// This process can fail; if so, it returns a false value.
///
/// \trigger{on_spawn}
///
/// \param c The operand creature.
/// \param z The zone to place the creature in.
/// \return Boolean success of the spawn.
///
int crtr_spawn(creature * c, zone * z)
{
	int timeout = SPAWN_TIMEOUT;
	int x, y;

	debug("Spawning %s", crtr_name(c));

	do {
		x = random() % z->width;
		y = random() % z->height;
		if(!(timeout--)) {
            c->z = z;
            crtr_death(c, "overcrowding");
            return 0;
		}
	} while (!crtr_tele(c, x, y, z));

	trigger_pull(&c->on_spawn, c, NULL);
	return 1;
}

///
/// Frees the creature if c->nofree not set
///
/// \note
/// At present, the creature must be deceased for this to work.
///
/// \warning
/// This does not remove the creature from the zone; use
/// \ref zone_rm_crtr for that.
///
/// \param c The creature to free.
///
void crtr_free(creature * c)
{
	if (c->refs != NOFREE && !--c->refs) {
		assert(c->deceased);

		inv_free(c->inv);
		free(c->generic_name);
		free(c->specific_name);
		free(c);
	}
}

///
/// Sets deceased, pulls the on_death trigger, and remove from
/// the zone.
///
/// \note
/// At present, the creature must be in a zone for this to work.
///
/// \trigger{on_death}
///
/// \param c The operand creature.
/// \param meth A string representing the type of death that occured.
///
void crtr_death(creature * c, char * meth)
{
	assert(c->z != NULL);

	debug("%s died from %s", crtr_name(c), meth);

	c->deceased = 1;
	trigger_pull(&c->on_death, c, meth);

	zone_rm_crtr(c->z, c);
}

///
/// This takes a creature, and tries to place it somewhere
///
/// Will fail if target square is:
/// * already occupied
/// * impassible
/// * nonexistant
///
/// \note
/// This will remove the creature from any previous zone it
/// was occupying.
///
/// \param crtr The creature to move
/// \param x The x coordinate.
/// \param y The y coordinate.
/// \param z The zone to move to.
/// \return Whether or not the move succeeded.
///
int crtr_tele(creature * crtr, int x, int y, zone * z)
{
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
			}

			if (z == PLYR.z) {
				zone_update(z, x, y);
			}

			disp_refresh();
		}

		// update coordinates
		crtr->x = x;
		crtr->y = y;
		crtr->z = z;

		return 1;
	}

	debug("%s failed to teleport to %s@%d,%d", crtr_name(crtr), zone_name(z), x, y);

	return 0;
}

///
/// Checks if a creature can move/teleport into a space
///
/// \param crtr The operant creature.
/// \param x The x coordinate.
/// \param y The y coordinate.
/// \param z The target zone.
/// \return Whether or not the creature can move to that destination.
///
int crtr_can_tele(creature * crtr, int x, int y, zone * z)
{
	tile * t = zone_at(z, x, y);
	return t != NULL && t->crtr == NULL && !t->impassible;
}

///
/// Simply wraps to \ref crtr_tele, but relative
///
/// \param crtr The operant creature.
/// \param dx Delta in the x direction.
/// \param dy Delta in the y direction.
/// \return Success of the move.
///
int crtr_move(creature * crtr, int dx, int dy)
{
	int nx = crtr->x + dx;
	int ny = crtr->y + dy;

	return crtr_tele(crtr, nx, ny, crtr->z);
}

///
/// Gives a creature experience points and levels up if merited
///
/// \trigger{on_lvlup}
///
/// \param c The operant creature.
/// \param xp The experience to award.
///
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

///
/// Unequips an item.
/// This never fails, even when no item is equipped there.
///
/// \param c The operant creature.
/// \param sl The slot to remove an item from.
///
void crtr_unequip(creature * c, slot sl)
{
	if (c->slots[sl] != NULL) {
		c->attack -= c->slots[sl]->modify_attack;
		c->ac     -= c->slots[sl]->modify_ac;
		c->slots[sl] = NULL;
	}
}

///
/// Equips an item, unequips whatever was there first
///
/// \param c The operant creature.
/// \param it The item to equip.
/// \param sl The slot to equip the item to.
/// \return Whether or not the equip succeeded.
///
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


///
/// The attacker attacks the defender (attack vs ac).
///
/// \note
/// Does not free any resources when the defender is killed.
///
/// \param attacker The attacking creature.
/// \param defender The defending creature.
/// \return The damage dealt, or `DEAD` if a the defender died.
///
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

///
/// Returns the creature name
///  specific>generic>'mysterious creature'
///
/// \param c The operant creature.
/// \return A pointer to a displayable name.
///
const char * crtr_name(const creature * c)
{
	if (c->specific_name != NULL) return c->specific_name;
	if (c->generic_name  != NULL) return c->generic_name;
	return "mysterious creature";
}

///
/// Returns the disposition creature a feels toward creature b.
/// This is a signed integer value whose sign indicates amiability
/// (positive) or hostility (negative) and whose magnitude is a
/// relative measure of the strength of the disposition. The unit
/// of this magnitude is arbitrarily defined.
///
/// As of now, only takes into consideration faction relations.
///
/// \param a The operant creature.
/// \param b The creature for which a's disposition is to be computed.
/// \return The disposition.
///
int crtr_disposition(const creature * a, const creature * b)
{
	if (a == b) return 1000; // temporary
	return fctn_relto(a->fctn, b->fctn);
}

///
/// Controls a creature as a basic beast-like ai
/// Attacks creatures based on closeness and disposition, perfering to attack
///   creatures close and of a negative disposition.
///
/// \param c The creature to simulate.
///
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

///
/// This is called once per game step, and is responsible for
/// performing basic accounting and simulation tasks.
///
/// \param c The creature to step.
/// \param steps The number of steps performed thus far.
///
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

///
/// If equipped, unequip item, then remove from inventory
///
/// \warning
/// Do not call \ref inv_rm on the creature's inventory yourself;
/// you could leave a dangling reference in the equipment.
///
/// \param c The operant creature.
/// \param i The index of the inventory.
/// \return The item thereby removed.
///
item * crtr_rm_item(creature * c, int i)
{
	item * it = c->inv->itms[i];

	if (item_equipped(it, c)) {
		crtr_unequip(c, it->slot);
	}

	inv_rm(it);
	return it;
}

///
/// Tests if a creature can dodge an item.
///
/// This performs a roll of 1d[c->reflex]; if this value is greater
/// than the difficulty, the dodge succeeds.
///
/// \param c The operant creature.
/// \param difficulty The measure of difficulty.
/// \return Whether the dodge succeeds.
///
int crtr_dodges(creature * c, int difficulty)
{
	int roll;
	if (difficulty < 0) difficulty = 0;
	roll = random() % c->reflex;
	return difficulty < roll || roll == c->reflex;
}

///
/// Change stance, see \ref creatures.h
///
/// \param c The operant creature.
/// \param s The stance to change to.
///
void crtr_stance(creature * c, int s){
	c->stance = s;
}


///
/// The following functions are typically called when actions are completed
///

///
/// Try to move (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param dx Delta in the x direction.
/// \param dy Delta in the y direction.
///
void crtr_try_move(creature * c, int dx, int dy)
{
	if (crtr_move(c, dx, dy)) {
		trigger_pull(&c->on_act_comp, c, NULL);
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_MOVE);
	}
}

///
/// Try to move, possibly attacking (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param dx Delta in the x direction.
/// \param dy Delta in the y direction.
///
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

///
/// Try to pick an item up from the currently occupied tile (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param i The index of the item in the tile's inventory.
///
void crtr_try_pickup(creature * c, int i)
{
	int j;
	tile * t = tileof(c);

	if (t->inv->size > i && t->inv->itms[i] != NULL) {
		if ((j = inv_add(c->inv, t->inv->itms[i])) != INVALID) {
			trigger_pull(&c->on_act_comp, c, c->inv->itms[j]);
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_PICKUP_HEAVY);
		}
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_PICKUP_PRESENT);
	}
}

///
/// Try to drop an item on the occupied tile (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param i The index of the item in the creature's inventory.
///
void crtr_try_drop(creature * c, int i)
{
	int j;
	tile * t = tileof(c);

	if (c->inv->size > i && c->inv->itms[i] != NULL) {
		if ((j = inv_add(t->inv, c->inv->itms[i])) != INVALID) {
			if (item_equipped(c->inv->itms[i], c)) {
				crtr_unequip(c, c->inv->itms[i]->slot);
			}

			trigger_pull(&c->on_act_comp, c, t->inv->itms[j]);
		} else {
			trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_DROP_HEAVY);
		}
	} else {
		trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_DROP_PRESENT);
	}
}

///
/// Try to consume an item (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param i The index of the consumable item in the creature's inventory.
///
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

///
/// Try to equip an item (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param i The index of the equippable item in the creature's inventory.
///
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

///
/// Try to throw an item (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param i The index of the throwable item in the creature's inventory.
/// \param dx Delta in the x direction of the throw.
/// \param dy Delta in the y direction of the throw.
///
void crtr_try_throw(creature * c, int i, int dx, int dy)
{
	assert(c->inv->size > i);
	assert(c->inv->itms[i] != NULL);

	if (i < c->inv->size && c->inv->itms[i] != NULL) {
		if (item_throw(c->inv->itms[i], c->x, c->y, c->z, dx, dy, c->throw)) {
			//crtr_rm_item(c, i);
			trigger_pull(&c->on_act_comp, c, NULL);
			return;
		}
	}

	trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_THROW);
}

///
/// Try to use a feature (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param dx Delta x to the usable feature.
/// \param dy Delta y to the usable feature.
///
void crtr_try_use(creature * c, int dx, int dy)
{
	assert(c != NULL);
	tile* t;
	t=&c->z->tiles[c->x+dx][c->y+dy];
	if (t->obj != NULL) {
		use_object[t->obj->type](t,c->x+dx,c->y+dy,c,c->z);
		return;
	}

	trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_USE);
}

///
/// Try to cast a spell (as the result of an action).
///
/// \trigger {on_act_comp,on_act_fail}
///
/// \param c The operant creature.
/// \param i The index of the spell in the creature's library.
/// \param argc The number of arguments in argv.
/// \param argv Arguments to the spell, as an array of strings.
///
void crtr_try_cast(creature * c, int i, int argc, void ** argv)
{
	if (i < c->lib->size && c->lib->spls[i] != NULL) {
		c->lib->spls[i]->effect(c->lib->spls[i], c, argc, argv);
		trigger_pull(&c->on_act_comp, c, NULL);
		return;
	}

	trigger_pull(&c->on_act_fail, c, V_ACT_FAIL_CAST);
}

///
/// The following function schedule creature actions
///
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

void crtr_act_use(creature * c, int x, int y)
{
	ACT_TMPLT(ACT_USE);
	a->p.dir.x   = x;
	a->p.dir.y   = y;
	schedule(a, c->speed);
}

void crtr_act_cast(creature * c, int i, int argc, void ** argv)
{
	ACT_TMPLT(ACT_CAST);
	a->p.parms.ind = i;
	a->p.parms.argc = argc;
	a->p.parms.argv = argv;
	schedule(a, c->speed);
}

void crtr_act_idle(creature * c)
{
	ACT_TMPLT(ACT_IDLE);
	schedule(a, c->speed);
}

