//
// zone.c
//

#include <assert.h>
#include <stdlib.h>
#include "item.h"
#include "zone.h"
#include "world.h"
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

static iform * adjforms[MAX_FORMS];

// 1 indicates presence of wall, -1 for not, 0 for either
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
		1,   -1,
		1, 1, 0,
	}, VLN },

	{ {
		 0, 1, 1,
		-1,    1,
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
		1,  1, 1,
		1,     1,
		0, -1, 0,
	}, HLN },

	{ {
		0, -1, 0,
		1,     1,
		1,  1, 1,
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

static iform * defform = NULL;

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

static iform * get_form(int ** walls, int x, int y, zone * z)
{
	int want;
	int i, j;
	int tx, ty;

	for (i = 0; i < countof(adjsty); i++) {
		for (j = 0; j < 8; j++) {
			if (adjsty[i].on[j]) {
				tx = x + adjind[j].dx;
				ty = y + adjind[j].dy;

				want = !!(adjsty[i].on[j] + 1);
				if (on(walls, tx, ty, z) != want) break;
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
	item * it;
	creature * cr;

	if (defform == NULL) {
		adjforms[LLC] = iform_new(USELESS, ACS_LLCORNER);
		adjforms[LRC] = iform_new(USELESS, ACS_LRCORNER);
		adjforms[ULC] = iform_new(USELESS, ACS_ULCORNER);
		adjforms[URC] = iform_new(USELESS, ACS_URCORNER);
		adjforms[VLN] = iform_new(USELESS, ACS_VLINE);
		adjforms[HLN] = iform_new(USELESS, ACS_HLINE);
		adjforms[LTE] = iform_new(USELESS, ACS_LTEE);
		adjforms[RTE] = iform_new(USELESS, ACS_RTEE);
		adjforms[TTE] = iform_new(USELESS, ACS_TTEE);
		adjforms[BTE] = iform_new(USELESS, ACS_BTEE);
		adjforms[CRS] = iform_new(USELESS, ACS_PLUS);
		adjforms[BLN] = iform_new(USELESS, ' ');

		for (i = 0; i < countof(adjforms); i++) {
			adjforms[i]->weight = TILE_MAX_WEIGHT;
		}

		defform = iform_new(USELESS, '#');
		defform->weight = TILE_MAX_WEIGHT;
	}

	// generate rooms
	rc = rand() % 10 + 10;
	rv = malloc(sizeof(room) * rc);

	for (i = 0; i < rc; i++) {
		rv[i].w = rand() % 12 + 5;
		rv[i].h = rand() % 8 + 3;
		rv[i].x = rand() % (z->width  - rv[i].w);
		rv[i].y = rand() % (z->height - rv[i].h);
	}

	// cut out rooms
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

	// draw walls
	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			if (walls[x][y]) {
				it = item_new(get_form(walls, x, y, z));
				inv_add(z->tiles[x][y].inv, it);
				z->tiles[x][y].impassible = 1;
			}
		}
	}

	// place some random junk
	if (world.iform_cnt != 0) {
		for (i = rand() % 10 + 5; i >= 0; i--) {
			it = item_new(world.iforms[rand() % world.iform_cnt]);

			do {
				x = rand() % z->width;
				y = rand() % z->height;
			} while (!inv_try(z->tiles[x][y].inv, it));

			item_tele(it, x, y, z);
		}
	}

	// place some more random junk
	if (world.cform_cnt != 0) {
		for (i = rand() % 10 + 5; i >= 0; i--) {
			cr = crtr_new(world.cforms[rand() % world.cform_cnt]);

			do {
				x = rand() % z->width;
				y = rand() % z->height;
			} while (z->tiles[x][y].crtr != NULL);

			crtr_tele(cr, x, y, z);
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

	z->tiles = malloc(sizeof(tile *) * w);
	for (i = 0; i < w; i++) {
		z->tiles[i] = malloc(sizeof(tile) * h);
		for (j = 0; j < h; j++) {
			z->tiles[i][j].impassible = 0;
			z->tiles[i][j].crtr = NULL;
			z->tiles[i][j].inv = inv_new(TILE_MAX_WEIGHT);
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
			// TODO free creature?
			inv_free(z->tiles[i][j].inv);
		}
		free(z->tiles[i]);
	}

	free(z->tiles);
	free(z);
}

void zone_update(zone * z, int x, int y)
{
	int i;
	int weight = -1;
	item * it;
	chtype ch = '.';

	if (z->tiles[x][y].crtr == NULL) {
		for (i = 0; i < z->tiles[x][y].inv->size; i++) {
			it = z->tiles[x][y].inv->itms[i];

			if (it != NULL && it->f->weight > weight) {
				weight = it->f->weight;
				ch = it->f->ch;
			}
		}
	} else {
		ch = z->tiles[x][y].crtr->f->ch;
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

tile * zone_at(zone * z, int x, int y)
{
	if (x < 0 || y < 0 || x >= z->width || y >= z->height) {
		return NULL;
	}
	return &z->tiles[x][y];
}
