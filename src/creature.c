//
// creature.c
//

#include <stdlib.h>
#include "zone.h"
#include "world.h"
#include "display.h"
#include "creature.h"

creature * crtr_new(cform * f)
{
	creature * c = malloc(sizeof(creature));

	c->nofree = 0;
	c->f = f;
	c->health = f->max_health;
	c->x = c->y = 0;
	c->z = NULL;
	c->ability = NULL;

	return c;
}

void crtr_free(creature * c)
{
	if (!c->nofree) {
		cform_free(c->f);
		free(c);
	}
}

static void update2(zone * z, int x1, int y1, int x2, int y2)
{
	if (z != NULL && z == world.plyr.z) {
		zone_update(z, x1, y1);
		zone_update(z, x2, y2);
		wrefresh(dispscr);
	}
}

int crtr_tele(creature * crtr, int x, int y, zone * z)
{
	if (x >= 0 && x < z->width 
		&& y >= 0 && y < z->height
		&& z->tiles[x][y].crtr == NULL
		&& !z->tiles[x][y].impassible)
	{
		z->tiles[x][y].crtr = crtr;

		if (crtr->z != NULL) {
			tileof(crtr)->crtr = NULL;
		}

		update2(crtr->z, x, y, crtr->x, crtr->y);

		crtr->x = x;
		crtr->y = y;
		crtr->z = z;

		return 1;
	}

	return 0;
}

int crtr_move(creature * crtr, int dx, int dy)
{
	int ox = crtr->x;
	int oy = crtr->y;
	int nx = ox + dx;
	int ny = oy + dy;

	return crtr_tele(crtr, nx, ny, crtr->z);
}
