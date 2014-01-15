//
// zone.c
//

#include <stdlib.h>
#include "log.h"
#include "zone.h"
#include "world.h"
#include "object.h"
#include "display.h"

typedef struct {
	int x, y, w, h;
} room;

static int in_room(room * r, int x, int y)
{
	return (r->x >= x && r->x < r->x + r->w)
		&& (r->y >= y && r->y < r->y + r->h);
}

static void generate(zone * z)
{
	int i, x, y;
	int rc;
	room * rv;
	object * o;

	rc = rand() % ((z->width * z->height) / 20);
	rv = malloc(sizeof(room) * rc);

	for (i = 0; i < rc; i++) {
		rv[i].w = rand() % 5 + 5;
		rv[i].h = rand() % 3 + 3;
		rv[i].x = rand() % (z->width  - rv[i].w);
		rv[i].y = rand() % (z->height - rv[i].h);
	}

	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			for (i = 0; i < rc; i++) {
				if (in_room(rv + i, x, y)) continue;
			}

			o = obj_new(USELESS, '#');
			o->x = x;
			o->y = y;
			o->z = z;
			z->objs[x][y] = o;
		}
	}
}

zone * zone_new(int w, int h)
{
	int i, j;
	zone * z;

	z = malloc(sizeof(zone));
	z->width = w;
	z->height = h;

	wrlog("w = %p", z);
	z->objs = malloc(sizeof(object **) * w);
	for (i = 0; i < w; i++) {
		z->objs[i] = malloc(sizeof(object *) * h);
		for (j = 0; j < h; j++) {
			z->objs[i][j] = NULL;
		}
	}

	generate(z);

	return z;
}

void zone_free(zone * z)
{
	int i, j;

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			obj_free(z->objs[i][j]);
		}
		free(z->objs[i]);
	}

	free(z->objs);
	free(z);
}

void zone_update(zone * z, int i, int j)
{
	chtype ch;

	if (z->objs[i][j] != NULL) {
		ch = z->objs[i][j]->ch;
	} else {
		ch = '.';
	}

	mvwaddch(dispscr, j, i, ch);
}

void zone_draw(zone * z)
{
	int i, j;

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			zone_update(z, i, j);
		}
	}

	wrefresh(dispscr);
}
