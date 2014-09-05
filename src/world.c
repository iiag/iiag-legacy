//
// world.c
//

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "log.h"
#include "util.h"
#include "world.h"
#include "config.h"
#include "player.h"
#include "inventory.h"
#include "io/display.h"

world_t world;

//
// Makes sure everything is in a usable state
// Only does something the first time it is called
//
void assure_world(void)
{
	static int first = 1;

	if (first) {
		info("Initializing world (assure_world)...");

		world.tm.era   = 3;
		world.tm.year  = 329;
		world.tm.month = 4;
		world.tm.mday  = 5;
		world.tm.wday  = 4;
		world.tm.hour  = 9;
		world.tm.min   = 0;


		crtr_init(&world.plyr, '@' | A_BOLD | COLOR_PAIR(COLOR_SELF));
		world.plyr.specific_name = "Player"; // XXX Is this a good idea?

		vector_init(&world.zones);

		world.gcrtrs = new_gclass(NULL);
		world.gitems = new_gclass(NULL);
		world.gmats  = new_gclass(NULL);
		world.grooms = new_gclass(NULL);

		// allocate 16 actions to start
		world.acts = malloc(sizeof(action_node) * 16);
		world.acts_cnt = 0;
		world.acts_alloc = 16;

		// load name data
		world.eth = load_ethnicity("names/misriyyun");

		first = 0;
	}
}

//
// Performs the first initialization of the world
// Will call assure_world if it has yet to be called
//
void init_world(void)
{
	zone * z;

	assure_world();

	z = zone_new(150, 50);
	vector_append(&world.zones, z);
	
	debug("Initializing player triggers...");

	#ifndef SERVER
	world.plyr.on_death.c_func    = (trigger_cfunc)plyr_ev_death;
	world.plyr.on_lvlup.c_func    = (trigger_cfunc)plyr_ev_lvlup;
	world.plyr.on_act_comp.c_func = (trigger_cfunc)plyr_ev_act_comp;
	world.plyr.on_act_fail.c_func = (trigger_cfunc)plyr_ev_act_fail;
	world.plyr.refs = NOFREE;

	crtr_spawn(&world.plyr, z);
	zone_update(z, world.plyr.x, world.plyr.y);
	#endif
}

//
// Right now, only steps the player's zone and updates the time
//
void step_world(void)
{
	int i;
	long step, step_diff;
	action * a;

	assert(world.plyr.health > 0);

	if (!world.acts_cnt) {
		step_diff = 0;
	} else {
		step = world.acts->step;
		assert(step >= world.step);
		step_diff = step - world.step;
		world.step = step;

		// process actions
		while (world.acts_cnt && world.acts->step == step) {
			a = pop_action();
			assert(a != NULL);

			if (!a->c->deceased) {
				switch (a->type) {
				case ACT_IDLE:
					trigger_pull(&a->c->on_act_comp, a->c, NULL);
					break;
				case ACT_MOVE:
					crtr_try_move(a->c, a->p.dir.x, a->p.dir.y);
					break;
				case ACT_AA_MOVE:
					crtr_try_aa_move(a->c, a->p.dir.x, a->p.dir.y);
					break;
				case ACT_PICKUP:
					crtr_try_pickup(a->c, a->p.ind);
					break;
				case ACT_DROP:
					crtr_try_drop(a->c, a->p.ind);
					break;
				case ACT_CONSUME:
					crtr_try_consume(a->c, a->p.ind);
					break;
				case ACT_EQUIP:
					crtr_try_equip(a->c, a->p.ind);
					break;
				case ACT_THROW:
					crtr_try_throw(a->c, a->p.throw.ind, a->p.throw.x, a->p.throw.y);
					break;
				}
			}

			a->c->act = NULL;
			crtr_free(a->c);
			free(a);
		}

		// update time
		if(!config.multiplayer)
		world.tm.steps += step_diff;

		while (world.tm.steps >= 6000) {
			world.tm.steps -= 6000;
			world.tm.min++;

			if (world.tm.min >= 60) {
				world.tm.min -= 60;
				world.tm.hour++;

				if (world.tm.hour >= 24) {
					world.tm.hour -= 24;
					world.tm.mday++;
					world.tm.wday = (world.tm.wday + 1) % 7 + 1;

					if (world.tm.mday > 30 + (world.tm.month & 1)) {
						world.tm.mday -= 30 + (world.tm.month & 1);
						world.tm.month++;

						if (world.tm.month > 12)
							world.tm.year++;
					}
				}
			}
		}
	}

	// update all zones
	for (i = 0; i < world.zones.cnt; i++) {
		zone_step(world.zones.arr[i], step_diff);
	}
}

//
// Fills the string to with the time
// 30 characters should be enough right now
//
void get_time(char * to, int max)
{
	snprintf(to, max, "%02d:%02d:%02d.%02d %d/%d/%d Era %d",
		world.tm.hour, world.tm.min, world.tm.steps/100, world.tm.steps%100,
		world.tm.month, world.tm.mday, world.tm.year,
		world.tm.era);
}

//
// All the following functions have to do with the action heap
//
static void swap(action_node * a, action_node * b)
{
	action_node tmp = *a;
	*a = *b;
	*b = tmp;
}

static int compare(action_node * a, action_node * b)
{
	// returns 1 if a should be higher in the heap, 0 for b
	// prioritize the player cause he's special

	assert(a->act != NULL);
	assert(b->act != NULL);
	if (a->act->c == &PLYR) return a->step <= b->step;
	return a->step < b->step;
}

static void heapify_up(int i)
{
	action_node * n;
	action_node * p;

	if (i) {
		n = world.acts + i;
		p = world.acts + (i-1)/2; // get the parent

		if (compare(n, p)) {
			swap(n, p);
			heapify_up(p - world.acts);
		}
	}
}

static void heapify_down(int i)
{
	int has_l, has_r;
	action_node * n;
	action_node * l;
	action_node * r;

	if (i < world.acts_cnt) {
		n = world.acts + i;
		l = world.acts + (i+1)*2-1; // get the left child
		r = world.acts + (i+1)*2  ; // get the right child

		has_l = l - world.acts < world.acts_cnt;
		has_r = r - world.acts < world.acts_cnt;

		if (has_l && (!has_r || compare(l, r)) && compare(l, n)) {
			swap(n, l);
			heapify_down(l - world.acts);
		}
		else if (has_r && compare(r, n)) {
			swap(n, r);
			heapify_down(r - world.acts);
		}
	}
}

void schedule(action * act, long step)
{
	action_node * a;

	if (world.acts_cnt == world.acts_alloc) {
		world.acts_alloc *= 2;
		world.acts = realloc(world.acts, world.acts_alloc * sizeof(action_node));
	}

	a = world.acts + world.acts_cnt;
	a->step = step + world.step;
	a->act  = act;

	heapify_up(world.acts_cnt++);
}

action * pop_action(void)
{
	action * a;

	if (world.acts_cnt) {
		a = world.acts->act;
		assert(a != NULL);

		swap(world.acts, world.acts + world.acts_cnt - 1);
		world.acts_cnt--;
		heapify_down(0);
	} else {
		a = NULL;
	}

	return a;
}
