//
// world.c
//

#include <stdlib.h>
#include <ncurses.h>
#include "world.h"
#include "display.h"
#include "inventory.h"

world_st world;
object * plyr;

void init_world(void)
{
	int x, y;

	world.zones = malloc(sizeof(zone *));
	*world.zones = zone_new(80, 25);
	world.plyr.ch = '@' | A_BOLD;
	world.plyr.weight = TILE_MAX_WEIGHT / 2;
	world.plyr.flags |= FL_NOFREE;

	do {
		x = rand() % world.zones[0]->width;
		y = rand() % world.zones[0]->height;
	} while (!inv_try(world.zones[0]->tiles[x][y], &world.plyr));

	obj_tele(&world.plyr, x, y, *world.zones);
	plyr = &world.plyr;
}
