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
	return x >= r->x && x < r->x + r->w
		&& y >= r->y && y < r->y + r->h;
}

static int on(int ** walls, int x, int y, zone * z)
{
	if (x < 0) return 1;
	if (y < 0) return 1;
	if (x >= z->width)  return 1;
	if (y >= z->height) return 1;
	return walls[x][y];
}

// this function is really ugly
static void generate(zone * z)
{
	int i, x, y;
	int rc;
	int ** walls;
	chtype ch;
	room * rv;
	object * o;

	rc = rand() % 10 + 5;
	rv = malloc(sizeof(room) * rc);

	for (i = 0; i < rc; i++) {
		rv[i].w = rand() % 12 + 5;
		rv[i].h = rand() % 8 + 3;
		rv[i].x = rand() % (z->width  - rv[i].w);
		rv[i].y = rand() % (z->height - rv[i].h);
	}

	walls = malloc(sizeof(int *) * z->width);
	for (x = 0; x < z->width; x++) {
		walls[x] = malloc(sizeof(int) * z->height);

		for (y = 0; y < z->height; y++) {
			for (i = 0; i < rc; i++) {
				if (in_room(rv + i, x, y)) break;
			}

			walls[x][y] = (i == rc);
		}
	}

	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			// upper left corner
			if (on(walls, x+1, y, z) && on(walls, x, y+1, z) && !on(walls, x-1, y, z) && !on(walls, x, y-1, z)) {
				ch = ACS_ULCORNER;
			}
			// upper right corner
			else if (!on(walls, x+1, y, z) && on(walls, x, y+1, z) && on(walls, x-1, y, z) && !on(walls, x, y-1, z)) {
				ch = ACS_URCORNER;
			}
			// bottom left corner
			else if (on(walls, x+1, y, z) && !on(walls, x, y+1, z) && !on(walls, x-1, y, z) && on(walls, x, y-1, z)) {
				ch = ACS_LLCORNER;
			}
			// bottom right corner
			else if (!on(walls, x+1, y, z) && !on(walls, x, y+1, z) && on(walls, x-1, y, z) && on(walls, x, y-1, z)) {
				ch = ACS_LRCORNER;
			}
			// h line
			else if (!on(walls, x, y+1, z) || !on(walls, x, y-1, z)) {
				ch = ACS_HLINE;
			}
			// v line
			else if (!on(walls, x+1, y, z) || !on(walls, x-1, y, z)) {
				ch = ACS_VLINE;
			}
			else {
				ch = ' ';
			}

			o = obj_new(USELESS, ch);
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
