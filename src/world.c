//
// world.c
//

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "log.h"
#include "util.h"
#include "world.h"
#include "display.h"
#include "inventory.h"
#include "form/form.h"

world_st world;

void assure_world(void)
{
	static int first = 1;

	if (first) {
		vector_init(&world.cforms);
		vector_init(&world.iforms);

		first = 0;
	}
}

void init_world(void)
{
	int x, y;
	cform * f;

	world.zones = malloc(sizeof(zone *));
	*world.zones = zone_new(150, 50);

	f = cform_new('@' | A_BOLD);
	f->weight = TILE_MAX_WEIGHT / 2;
	f->max_health = 10;

	crtr_init(&world.plyr, f);
	world.plyr.nofree = 1;
	world.plyr.inv = inv_new(500);
	world.plyr.attack = 5;

	do {
		x = random() % get_dispw();
		y = random() % get_disph();
	} while (!crtr_tele(&world.plyr, x, y, *world.zones));

	zone_update(world.plyr.z, x, y);
}
