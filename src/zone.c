//
// zone.c
//

#include <stdlib.h>
#include "log.h"
#include "zone.h"
#include "world.h"
#include "object.h"
#include "display.h"
#include "inventory.h"

#define countof(X) (sizeof(X) / sizeof(*X))

// begin adjacent walls style
static struct {
	int dx, dy;
} adjind[] = {
	{ -1,  1 }, // lower left
	{  0,  1 }, // lower middle
	{  1,  1 }, // lower right
	{ -1,  0 }, // middle left
	{  1,  0 }, // middle right
	{ -1, -1 }, // upper left
	{  0, -1 }, // upper middle
	{  1, -1 }  // upper right
};

static chtype adjchs[11];

static struct {
	int on[8];
	int ci;
} adjsty[] = {
	// lower left corner
	{ { 0,0,0, 0,1, 0,1,0 }, 0 },

	{ { 0,0,0, 0,1, 0,1,1 }, 0 },
	{ { 0,0,0, 0,1, 1,1,1 }, 0 },
	{ { 0,0,1, 0,1, 0,1,1 }, 0 },

	{ { 1,1,1, 1,1, 1,1,0 }, 0 },

	// lower right corner
	{ { 0,0,0, 1,0, 0,1,0 }, 1 },

	{ { 0,0,0, 1,0, 1,1,0 }, 1 },
	{ { 1,0,0, 1,0, 1,1,0 }, 1 },
	{ { 0,0,0, 1,0, 1,1,1 }, 1 },

	{ { 1,1,1, 1,1, 0,1,1 }, 1 },

	// upper left corner
	{ { 0,1,0, 0,1, 0,0,0 }, 2 },

	{ { 0,1,1, 0,1, 0,0,0 }, 2 },
	{ { 1,1,1, 0,1, 0,0,0 }, 2 },
	{ { 0,1,1, 0,1, 0,0,1 }, 2 },

	{ { 1,1,0, 1,1, 1,1,1 }, 2 },

	// upper right corner
	{ { 0,1,0, 1,0, 0,0,0 }, 3 },

	{ { 1,1,0, 1,0, 0,0,0 }, 3 },
	{ { 1,1,1, 1,0, 0,0,0 }, 3 },
	{ { 1,1,0, 1,0, 0,0,1 }, 3 },

	{ { 0,1,1, 1,1, 1,1,1 }, 3 },

	// h line
	{ { 0,1,0, 0,0, 0,1,0 }, 4 },
	{ { 0,0,0, 0,0, 0,1,0 }, 4 },
	{ { 0,1,0, 0,0, 0,0,0 }, 4 },

	{ { 1,1,0, 0,0, 0,1,0 }, 4 },
	{ { 0,1,1, 0,0, 0,1,0 }, 4 },
	{ { 0,1,0, 0,0, 1,1,0 }, 4 },
	{ { 0,1,0, 0,0, 0,1,1 }, 4 },

	{ { 0,1,1, 0,1, 0,1,1 }, 4 },
	{ { 1,1,1, 0,1, 0,1,1 }, 4 },
	{ { 0,1,1, 0,1, 1,1,1 }, 4 },
	{ { 1,1,1, 0,1, 1,1,1 }, 4 },

	{ { 1,1,0, 1,0, 1,1,0 }, 4 },
	{ { 1,1,1, 1,0, 1,1,0 }, 4 },
	{ { 1,1,0, 1,0, 1,1,1 }, 4 },
	{ { 1,1,1, 1,0, 1,1,1 }, 4 },

	// vline
	{ { 0,0,0, 1,1, 0,0,0 }, 5 },
	{ { 0,0,0, 0,1, 0,0,0 }, 5 },
	{ { 0,0,0, 1,0, 0,0,0 }, 5 },

	{ { 1,0,0, 1,1, 0,0,0 }, 5 },
	{ { 0,0,1, 1,1, 0,0,0 }, 5 },
	{ { 0,0,0, 1,1, 1,0,0 }, 5 },
	{ { 0,0,0, 1,1, 0,0,1 }, 5 },

	{ { 1,1,1, 1,1, 0,0,0 }, 5 },
	{ { 1,1,1, 1,1, 1,0,0 }, 5 },
	{ { 1,1,1, 1,1, 0,0,1 }, 5 },
	{ { 1,1,1, 1,1, 1,0,1 }, 5 },

	{ { 0,0,0, 1,1, 1,1,1 }, 5 },
	{ { 1,0,0, 1,1, 1,1,1 }, 5 },
	{ { 0,0,1, 1,1, 1,1,1 }, 5 },
	{ { 1,0,1, 1,1, 1,1,1 }, 5 },

	// left tee
	{ { 0,1,0, 0,1, 0,1,0 }, 6 },
	{ { 1,1,0, 1,1, 1,1,0 }, 6 },

	// right tee
	{ { 0,1,0, 1,0, 0,1,0 }, 7 },
	{ { 0,1,1, 1,1, 0,1,1 }, 7 },

	// top tee
	{ { 0,1,0, 1,1, 0,0,0 }, 8 },
	{ { 0,1,0, 1,1, 1,1,1 }, 8 },

	// bottom tee
	{ { 0,0,0, 1,1, 0,1,0 }, 9 },
	{ { 1,1,1, 1,1, 0,1,0 }, 9 },

	// space
	{ { 1,1,1, 1,1, 1,1,1 }, 10 },
};

static int defsty = '#';
//// end adjacent walls style ////

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
	static int init = 0;

	int i, j;
	int x, y;
	int tx, ty;
	int rc;
	int ** walls;
	chtype ch;
	room * rv;
	object * o;

	if (!init) {
		adjchs[0] = ACS_LLCORNER;
		adjchs[1] = ACS_LRCORNER;
		adjchs[2] = ACS_ULCORNER;
		adjchs[3] = ACS_URCORNER;
		adjchs[4] = ACS_VLINE;
		adjchs[5] = ACS_HLINE;
		adjchs[6] = ACS_LTEE;
		adjchs[7] = ACS_RTEE;
		adjchs[8] = ACS_TTEE;
		adjchs[9] = ACS_BTEE;
		adjchs[10] = ' ';
		init = 1;
	}

	rc = rand() % 10 + 10;
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
			if (walls[x][y]) {
				// this is not quite so awful

				ch = defsty;
				for (i = 0; i < countof(adjsty); i++) {
					for (j = 0; j < 8; j++) {
						tx = x + adjind[j].dx;
						ty = y + adjind[j].dy;
						if (on(walls, tx, ty, z) != adjsty[i].on[j]) break;
					}

					if (j == 8) ch = adjchs[adjsty[i].ci];
				}

				o = obj_new(USELESS, ch);
				o->x = x;
				o->y = y;
				o->z = z;
				o->weight = TILE_MAX_WEIGHT;
				inv_add(z->tiles[x][y], o);
			}
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

	z->tiles = malloc(sizeof(inventory **) * w);
	for (i = 0; i < w; i++) {
		z->tiles[i] = malloc(sizeof(inventory *) * h);
		for (j = 0; j < h; j++) {
			z->tiles[i][j] = inv_new(TILE_MAX_WEIGHT);
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
			inv_free(z->tiles[i][j]);
		}
		free(z->tiles[i]);
	}

	free(z->tiles);
	free(z);
}

void zone_update(zone * z, int x, int y)
{
	int i;
	object * ob;
	chtype ch = '.';
	obj_type ty = NONE;

	for (i = 0; i < z->tiles[x][y]->size; i++) {
		ob = z->tiles[x][y]->objs[i];
		if (ob != NULL && ob->type > ty) {
			ty = ob->type;
			ch = ob->ch;
		}
	}

	mvwaddch(dispscr, y, x, ch);
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
