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
		world.plyr_form = cform_new('@' | A_BOLD);

		vector_init(&world.cforms);
		vector_init(&world.iforms);
		vector_init(&world.zones);

		first = 0;
	}
}

void init_world(void)
{
	zone * z;

	assure_world();

	z = zone_new(150, 50);
	vector_append(&world.zones, z);

	crtr_init(&world.plyr, world.plyr_form);
	world.plyr.nofree = 1;
	world.plyr.inv = inv_new(500);

	crtr_spawn(&world.plyr, z);
	zone_update(z, world.plyr.x, world.plyr.y);
}
