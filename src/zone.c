//
// zone.c
//

#include <stdlib.h>
#include "form.h"
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

static form * adjforms[11];

static struct {
	int on[8];
	int fi;
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
	{ { 0,1,1, 0,1, 0,1,0 }, 6 },
	{ { 0,1,0, 0,1, 0,1,1 }, 6 },
	{ { 1,1,0, 1,1, 1,1,0 }, 6 },
	{ { 1,1,1, 1,1, 1,1,0 }, 6 },
	{ { 1,1,0, 1,1, 1,1,1 }, 6 },

	// right tee
	{ { 0,1,0, 1,0, 0,1,0 }, 7 },
	{ { 1,1,0, 1,0, 0,1,0 }, 7 },
	{ { 0,1,0, 1,0, 1,1,0 }, 7 },
	{ { 0,1,1, 1,1, 0,1,1 }, 7 },
	{ { 1,1,1, 1,1, 0,1,1 }, 7 },
	{ { 0,1,1, 1,1, 1,1,1 }, 7 },

	// top tee
	{ { 0,1,0, 1,1, 0,0,0 }, 8 },
	{ { 1,1,0, 1,1, 0,0,0 }, 8 },
	{ { 0,1,1, 1,1, 0,0,0 }, 8 },
	{ { 0,1,0, 1,1, 1,1,1 }, 8 },
	{ { 1,1,0, 1,1, 1,1,1 }, 8 },
	{ { 0,1,1, 1,1, 1,1,1 }, 8 },

	// bottom tee
	{ { 0,0,0, 1,1, 0,1,0 }, 9 },
	{ { 0,0,0, 1,1, 1,1,0 }, 9 },
	{ { 0,0,0, 1,1, 0,1,1 }, 9 },
	{ { 1,1,1, 1,1, 0,1,0 }, 9 },
	{ { 1,1,1, 1,1, 1,1,0 }, 9 },
	{ { 1,1,1, 1,1, 0,1,1 }, 9 },

	// space
	{ { 1,1,1, 1,1, 1,1,1 }, 10 },
};

static form * defform = NULL;
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
	int i, j;
	int x, y;
	int tx, ty;
	int rc;
	int ** walls;
	form * fm;
	room * rv;
	object * o;

	if (defform == NULL) {
		adjforms[0] = form_new(USELESS, ACS_LLCORNER);
		adjforms[1] = form_new(USELESS, ACS_LRCORNER);
		adjforms[2] = form_new(USELESS, ACS_ULCORNER);
		adjforms[3] = form_new(USELESS, ACS_URCORNER);
		adjforms[4] = form_new(USELESS, ACS_VLINE);
		adjforms[5] = form_new(USELESS, ACS_HLINE);
		adjforms[6] = form_new(USELESS, ACS_LTEE);
		adjforms[7] = form_new(USELESS, ACS_RTEE);
		adjforms[8] = form_new(USELESS, ACS_TTEE);
		adjforms[9] = form_new(USELESS, ACS_BTEE);
		adjforms[10] = form_new(USELESS, ' ');

		for (i = 0; i < countof(adjforms); i++) {
			adjforms[i]->weight = TILE_MAX_WEIGHT;
		}

		defform = form_new(USELESS, '#');
		defform->weight = TILE_MAX_WEIGHT;
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

				fm = defform;
				for (i = 0; i < countof(adjsty); i++) {
					for (j = 0; j < 8; j++) {
						tx = x + adjind[j].dx;
						ty = y + adjind[j].dy;
						if (on(walls, tx, ty, z) != adjsty[i].on[j]) break;
					}

					if (j == 8) {
						fm = adjforms[adjsty[i].fi];
						break;
					}
				}

				o = obj_new(fm);
				o->x = x;
				o->y = y;
				o->z = z;
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
		if (ob != NULL && ob->f->type > ty) {
			ty = ob->f->type;
			ch = ob->f->ch;
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
