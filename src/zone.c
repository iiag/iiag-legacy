//
// zone.c
//

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "item.h"
#include "zone.h"
#include "world.h"
#include "display.h"
#include "inventory.h"
#include "walls.h"

#define ROOM_INFREQ 40
#define ITEM_INFREQ 80
#define CRTR_INFREQ 100
#define ROOM_MIN 100
#define ITEM_MIN 40
#define CRTR_MIN 15

typedef struct {//specifies room with x and y coordinates adn w and h for width and height
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

static void set_wall_char(int ** walls, zone * z, int x, int y)
{
	int i, j;
	unsigned char ch = 0;

	for (j = 1; j >= -1; j--) {
		for (i = 1; i >= -1; i--) {
			if (i != 0 || j != 0) {
				ch <<= 1;
				ch |= !!on(walls, x + i, y + j, z);
			}
		}
	}

	z->tiles[x][y].ch = wall_chars[ch];
}


// this function is really ugly
static void generate(zone * z)
{
	static int first = 1;

	int i, x, y, max;
	int rc;
	int ** walls;
	room * rv;
	item * it;
	creature * cr;

	if (first) {
		fill_walls();
		first = 0;
	}

	// generate rooms
	rc = random() % ((z->width * z->height) / ROOM_INFREQ) + ROOM_MIN;
	rv = malloc(sizeof(room) * rc);

	for (i = 0; i < rc; i++) {
		rv[i].w = random() % 12 + 5;
		rv[i].h = random() % 8 + 3;
		rv[i].x = random() % (z->width  - rv[i].w - 1) + 1;
		rv[i].y = random() % (z->height - rv[i].h - 1) + 1;
	}


	// cut out rooms
	walls = malloc(sizeof(int *) * z->width);
	for (x = 0; x < z->width; x++) {
		walls[x] = malloc(sizeof(int) * z->height);

		for (y = 0; y < z->height; y++) {
			for (i = 0; i < rc; i++) {
				if (in_room(rv + i, x, y)) break;
			}
			z->tiles[x][y].ch = '.';
			walls[x][y] = (i == rc);
		}
	}


	// draw walls
	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			if (walls[x][y]) {
				z->tiles[x][y].impassible = 1;
				set_wall_char(walls,z,x,y);
			}
		}
	}

	// place some random junk
	if (world.iform_cnt != 0) {
		max = random() % (z->width * z->height / ITEM_INFREQ) + ITEM_MIN;
		for (i = max; i >= 0; i--) {
			it = item_new(world.iforms[random() % world.iform_cnt]);

			do {
				x = random() % z->width;
				y = random() % z->height;
			} while (z->tiles[x][y].impassible || !inv_try(z->tiles[x][y].inv, it));

			item_tele(it, x, y, z);
			zone_update(z, x, y);
		}
	}

	// place some more random junk
	if (world.cform_cnt != 0) {
		max = random() % (z->width * z->height / CRTR_INFREQ) + CRTR_MIN;
		for (i = max; i >= 0; i--) {
			cr = crtr_new(world.cforms[random() % world.cform_cnt]);

			do {
				x = random() % z->width;
				y = random() % z->height;
			} while (z->tiles[x][y].impassible || z->tiles[x][y].crtr != NULL);

			crtr_tele(cr, x, y, z);
			zone_update(z, x, y);
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
			z->tiles[i][j].show = 0;
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

void zone_draw_tile(zone * z, int x, int y)
{
	if (zone_can_see(z, world.plyr.x, world.plyr.y, x, y, 20)) {
		disp_put(x, y, z->tiles[x][y].ch);
	} else {
		disp_put(x, y, ' ');
	}
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

	z->tiles[x][y].ch = ch;
	zone_draw_tile(z, x, y);
}

void zone_draw(zone * z)
{
	int i, j;

	wclear(dispscr);

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			zone_draw_tile(z, i, j);
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

void zone_step(zone * z, int step)
{
	int x, y;

	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			if (z->tiles[x][y].crtr != NULL) {
				crtr_step(z->tiles[x][y].crtr, step);
			}
		}
	}
}

int zone_can_see(zone * z, int x0, int y0, int x1, int y1, int dist)
{
	if (x1 == x0 && y1 == y0) return 1;

	int x, y;
	float dx = (float) abs(x1 - x0);
	float dy = (float) abs(y1 - y0);

	if (sqrt(dx * dx + dy * dy) > (float) dist) return 0;

	int mx = ((x0 < x1) << 1) - 1;
	int my = ((y0 < y1) << 1) - 1;

	float err = 0;
	float derr;

	if (dx > dy) {
		assert(dx != 0);
		derr = fabs(dy / dx);

		for (x = x0, y = y0; x != x1; x += mx) {
			if (x != x0 && z->tiles[x][y].impassible) return 0;
			err += derr;

			if (err >= 0.5) {
				y += my;
				err -= 1.0;
			}
		}
	} else {
		assert(dy != 0);
		derr = fabs(dx / dy);

		for (x = x0, y = y0; y != y1; y += my) {
			if (y != y0 && z->tiles[x][y].impassible) return 0;
			err += derr;

			if (err >= 0.5) {
				x += mx;
				err -= 1.0;
			}
		}
	}

	return 1;
}
