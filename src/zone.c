//
// zone.c
//

#include <assert.h>
#include <stdlib.h>
#include "form.h"
#include "zone.h"
#include "world.h"
#include "object.h"
#include "display.h"
#include "inventory.h"

#define countof(X) (sizeof(X) / sizeof(*X))

static struct {
	int dx, dy;
} adjind[] = {
	{ -1, -1 }, // upper left
	{  0, -1 }, // upper middle
	{  1, -1 }, // upper right
	{ -1,  0 }, // middle left
	{  1,  0 }, // middle right
	{ -1,  1 }, // lower left
	{  0,  1 }, // lower middle
	{  1,  1 }, // lower right
};

enum {
	// for indexing adjforms
	LLC, LRC,
	ULC, URC,
	VLN, HLN,
	LTE, RTE,
	TTE, BTE,
	CRS,
	BLN,
	MAX_FORMS
};

static form * adjforms[MAX_FORMS];

// 1 indicates presence of wall, 0 possibly
static struct {
	int on[8];
	int fi;
} adjsty[] = {
	// blank
	{ {
		1, 1, 1,
		1,    1,
		1, 1, 1,
	}, BLN },

	// corners
	{ {
		0, 1, 1,
		1,    1,
		1, 1, 1,
	}, LRC },

	{ {
		1, 1, 0,
		1,    1,
		1, 1, 1,
	}, LLC },

	{ {
		1, 1, 1,
		1,    1,
		0, 1, 1,
	}, URC },

	{ {
		1, 1, 1,
		1,    1,
		1, 1, 0,
	}, ULC },

	// vline overriding left/right tees
	{ {
		1, 1, 0,
		1,    0,
		1, 1, 0,
	}, VLN },

	{ {
		0, 1, 1,
		0,    1,
		0, 1, 1,
	}, VLN },

	// left/right tees
	{ {
		0, 1, 0,
		1,    0,
		0, 1, 0,
	}, RTE },

	{ {
		0, 1, 0,
		0,    1,
		0, 1, 0,
	}, LTE },

	// hline overriding bot/top tees
	{ {
		1, 1, 1,
		1,    1,
		0, 0, 0,
	}, HLN },

	{ {
		0, 0, 0,
		1,    1,
		1, 1, 1,
	}, HLN },

	// top/bot tees
	{ {
		0, 1, 0,
		1,    1,
		0, 0, 0,
	}, BTE },

	{ {
		0, 0, 0,
		1,    1,
		0, 1, 0,
	}, TTE },

	// more corners
	{ {
		0, 1, 0,
		1,    0,
		0, 0, 0,
	}, LRC },

	{ {
		0, 1, 0,
		0,    1,
		0, 0, 0,
	}, LLC },

	{ {
		0, 0, 0,
		1,    0,
		0, 1, 0,
	}, URC },

	{ {
		0, 0, 0,
		0,    1,
		0, 1, 0,
	}, ULC },

	// cross
	{ {
		0, 1, 0,
		1,    1,
		0, 1, 0,
	}, CRS },

	// vline/hline
	{ {
		0, 0, 0,
		0,    0,
		0, 1, 0,
	}, VLN },

	{ {
		0, 1, 0,
		0,    0,
		0, 0, 0,
	}, VLN },

	{ {
		0, 0, 0,
		1,    0,
		0, 0, 0,
	}, HLN },

	{ {
		0, 0, 0,
		0,    1,
		0, 0, 0,
	}, HLN },
};

static form * defform = NULL;

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

static form * get_form(int ** walls, int x, int y, zone * z)
{
	int i, j;
	int tx, ty;

	for (i = 0; i < countof(adjsty); i++) {
		for (j = 0; j < 8; j++) {
			if (adjsty[i].on[j]) {
				assert(adjsty[i].on[j] == 1);

				tx = x + adjind[j].dx;
				ty = y + adjind[j].dy;

				if (!on(walls, tx, ty, z)) break;
			}
		}

		if (j == 8) return adjforms[adjsty[i].fi];
	}

	return defform;
}

// this function is really ugly
static void generate(zone * z)
{
	int i, x, y;
	int rc;
	int ** walls;
	room * rv;
	object * o;

	if (defform == NULL) {
		adjforms[LLC] = form_new(USELESS, ACS_LLCORNER);
		adjforms[LRC] = form_new(USELESS, ACS_LRCORNER);
		adjforms[ULC] = form_new(USELESS, ACS_ULCORNER);
		adjforms[URC] = form_new(USELESS, ACS_URCORNER);
		adjforms[VLN] = form_new(USELESS, ACS_VLINE);
		adjforms[HLN] = form_new(USELESS, ACS_HLINE);
		adjforms[LTE] = form_new(USELESS, ACS_LTEE);
		adjforms[RTE] = form_new(USELESS, ACS_RTEE);
		adjforms[TTE] = form_new(USELESS, ACS_TTEE);
		adjforms[BTE] = form_new(USELESS, ACS_BTEE);
		adjforms[CRS] = form_new(USELESS, ACS_PLUS);
		adjforms[BLN] = form_new(USELESS, ' ');

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

				o = obj_new(get_form(walls, x, y, z));
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
