//
// object.c
//

#include <stdlib.h>
#include <ncurses.h>
#include "form.h"
#include "world.h"
#include "object.h"
#include "display.h"
#include "inventory.h"

#define MAX_WEIGHT 500

object * obj_new(form * f)
{
	object * o = malloc(sizeof(object));
	o->f = form_copy(f);
	o->x = 0;
	o->y = 0;
	o->i = 0;
	o->z = NULL;
	o->flags = 0;
	o->inv = (f->flags & FL_HASINV) ? inv_new(MAX_WEIGHT) : NULL;
	o->health = f->max_health;
	return o;
}

void obj_free(object * o)
{
	if (~o->flags & FL_NOFREE) {
		form_free(o->f);
		free(o);
	}
}

static void update2(zone * z, int x1, int y1, int x2, int y2)
{
	if (z != NULL && z == world.plyr.obj->z) {
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

		if (crtr->obj->z != NULL) {
			tileof(crtr->obj)->crtr = NULL;
		}

		update2(crtr->obj->z, x, y, crtr->obj->x, crtr->obj->y);

		crtr->obj->x = x;
		crtr->obj->y = y;
		crtr->obj->z = z;

		return 1;
	}

	return 0;
}

int crtr_move(creature * crtr, int dx, int dy)
{
	int ox = crtr->obj->x;
	int oy = crtr->obj->y;
	int nx = ox + dx;
	int ny = oy + dy;

	return crtr_tele(crtr, nx, ny, crtr->obj->z);
}

int item_tele(object * obj, int x, int y, zone * z)
{
	int i = inv_add(z->tiles[x][y].inv, obj);

	if (i != INVALID) {
		if (obj->z != NULL) {
			inv_rm(z->tiles[x][y].inv, obj->i);
		}

		update2(obj->z, obj->x, obj->y, x, y);

		obj->x = x;
		obj->y = y;
		obj->z = z;
		obj->i = i;

		return 1;
	}

	return 0;
}

int item_move(object * obj, int dx, int dy)
{
	int i;
	int nx, ny;
	int ox = obj->x;
	int oy = obj->y;

	if (oy + dy >= 0 && oy + dy < obj->z->height
		&& ox + dx >= 0 &&ox + dx < obj->z->width)
	{
		nx = ox + dx;
		ny = oy + dy;

		i = inv_add(obj->z->tiles[nx][ny].inv, obj);
		if (i == INVALID) return 0;
		inv_rm(obj->z->tiles[ox][oy].inv, obj->i);

		obj->x = nx;
		obj->y = ny;
		obj->i = i;

		update2(obj->z, nx, ny, ox, oy);
		wrefresh(dispscr);
		return 1;
	}

	return 0;
}
