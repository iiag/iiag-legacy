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
	world.zones = malloc(sizeof(zone *));
	*world.zones = zone_new(80, 25);
	world.plyr.ch = '@' | A_BOLD;
	obj_tele(&world.plyr, 10, 10, *world.zones);
	plyr = &world.plyr;
}
