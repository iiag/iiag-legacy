//
// world.c
//

#include <stdlib.h>
#include <ncurses.h>
#include "world.h"
#include "display.h"

world_st world;
object * plyr;

void init_world(void)
{
	int x, y;

	world.zones = malloc(sizeof(zone *));
	*world.zones = zone_new(80, 25);
	world.plyr.ch = '@' | A_BOLD;

	do {
		x = rand() % world.zones[0]->width;
		y = rand() % world.zones[0]->height;
	} while (world.zones[0]->objs[x][y] != NULL);

	obj_tele(&world.plyr, x, y, *world.zones);
	plyr = &world.plyr;
}
