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

object * obj_new(form * f)
{
	object * o = malloc(sizeof(object));
	o->f = form_copy(f);
	o->x = 0;
	o->y = 0;
	o->i = 0;
	o->z = NULL;
	o->flags = 0;
	return o;
}

void obj_free(object * o)
{
	if (~world.plyr.flags & FL_NOFREE) {
		form_free(o->f);
		free(o);
	}
}

void obj_tele(object * obj, int x, int y, zone * z)
{
	int i = inv_add(z->tiles[x][y], obj);

	if (i != INVALID) {
		if (obj->z != NULL) {
			inv_rm(z->tiles[x][y], obj->i);
		}

		obj->x = x;
		obj->y = y;
		obj->z = z;
		obj->i = i;
	}
}

void obj_move(object * obj, int dx, int dy)
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

		i = inv_add(obj->z->tiles[nx][ny], obj);
		if (i == INVALID) return;
		inv_rm(obj->z->tiles[ox][oy], obj->i);

		obj->x = nx;
		obj->y = ny;

		zone_update(obj->z, nx, ny);
		zone_update(obj->z, ox, oy);
		wrefresh(dispscr);
	}
}
