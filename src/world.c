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
#include "player.h"
#include "display.h"
#include "inventory.h"
#include "form/form.h"

world_st world;

//
// Makes sure everything is in a usable state
// Only does something the first time it is called
//
void assure_world(void)
{
	static int first = 1;

	if (first) {
		world.tm.era   = 3;
		world.tm.year  = 329;
		world.tm.month = 4;
		world.tm.mday  = 5;
		world.tm.wday  = 4;
		world.tm.hour  = 9;
		world.tm.min   = 0;

		world.plyr_form = cform_new('@' | A_BOLD);

		vector_init(&world.cforms);
		vector_init(&world.iforms);
		vector_init(&world.zones);

		world.max_iforms_freq = 0;
		world.max_cforms_freq = 0;

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

	world.plyr_form->on_death.c_func = (trigger_cfunc)plyr_ev_death;
	world.plyr_form->on_lvlup.c_func = (trigger_cfunc)plyr_ev_lvlup;
	crtr_init(&world.plyr, world.plyr_form);
	world.plyr.nofree = 1;
	world.plyr.inv = inv_new(500);

	crtr_spawn(&world.plyr, z);
	zone_update(z, world.plyr.x, world.plyr.y);
}

//
// Right now, only steps the player's zone and updates the time
//
void step_world(void)
{
	static int step = 0;

	assert(world.plyr.health > 0);

	zone_step(world.plyr.z, step);
	step = !step;

	// update time
	world.tm.min += 5;

	while (world.tm.min >= 60) {
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

//
// Fills the string to with the time
// 30 characters should be enough right now
//
void get_time(char * to, int max)
{
	snprintf(to, max, "%02d:%02d %d/%d/%d Era %d",
		world.tm.hour, world.tm.min,
		world.tm.month, world.tm.mday, world.tm.year,
		world.tm.era);
}
