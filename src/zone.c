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

typedef struct {
	int xmin,xmax,ymin,ymax;
} partition;


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

static void pick_random_subregion(zone * z,partition * p, int * ret)
{
	int x, y;
	partition sqr = {0};

	sqr.xmin = ~sqr.xmin;
	sqr.ymin = ~sqr.ymin;

	for (y = p->ymin; y < p->ymax; y++) {
		for (x = p->xmin; x < p->xmax; x++) {
			// TODO: Optimize this
			if (z->tiles[x][y].type & TILE_WALL) {
				if (x < sqr.xmin)
					sqr.xmin = x;
				else if (x > sqr.xmax)
					sqr.xmax = x;

				if (y < sqr.ymin)
					sqr.ymin = y;
				else if (y > sqr.ymax)
					sqr.ymax = y;

			}
		}
	}

	// TODO: Check bounds
	ret[0] = random() % (sqr.xmax - sqr.xmin) + sqr.xmin;
	ret[1] = random() % (sqr.ymax - sqr.ymin) + sqr.ymin;

}

// Generate a random region in the given boundary
static void generate_region(zone * z, partition * p)
{
	// Find the center of the partition
	int cx = ((p->xmax - p->xmin) / 2) + p->xmin;
	int cy = ((p->ymax - p->ymin) / 2) + p->ymin;
	
	// Number of subsquares to generate the room
	int squares = rand() % 5 + 3;

	int i,j,s,rad;
	int r[2];


	// Do the initial starting square	
	for (i = -2; i <= 2; i++) {
		for (j = -2; j <= 2; j++) {
			z->tiles[cx+j][cy+i].type = TILE_FLOOR;
			z->tiles[cx+j][cy+i].ch   = '.';
		}
	}

	// TODO: check bounds
	for(s = 0; s < squares; s++) {
		pick_random_subregion(z,p,r); 
		rad = random() % 3 + 1;
	
		// TODO: allow rectangles
		for (i = -rad; i <= rad; i++) {
			for (j = -rad; j <=rad; j++) {
				z->tiles[r[0]+j][r[1]+i].type = TILE_FLOOR;
				z->tiles[r[0]+j][r[1]+i].ch   = '.';
			}
		}
	
	}

}


static void generate(zone * z)
{
	int xparts,yparts;
	int i,j;
	int x,y;

	// TODO: Randomly pick these numbers
	xparts = yparts = 3;
	int partnum = xparts*yparts;

	partition * parts = (partition*) malloc(sizeof(partition)*xparts*yparts);

	// TODO: randomly select partitions (non-deterministic)	
	parts[0].xmin = parts[1].xmin = parts[2].xmin = 0;
	parts[0].xmax = parts[1].xmax = parts[2].xmax = (z->width/3);
	
	parts[3].xmin = parts[4].xmin = parts[5].xmin = (z->width/3);
	parts[3].xmax = parts[4].xmax = parts[5].xmax = ((2*z->width)/3);

	parts[6].xmax = parts[7].xmax = parts[8].xmax = ((2*z->width)/3);
	parts[6].xmax = parts[7].xmax = parts[8].xmax = 2*z->width-1;

	parts[0].ymin = parts[3].ymin = parts[6].ymin = 0;
	parts[0].ymax = parts[3].ymax = parts[6].ymax = (z->height/3);
	
	parts[1].ymin = parts[4].ymin = parts[7].ymin = (z->height/3);
	parts[1].ymax = parts[4].ymax = parts[7].ymax = ((2*z->height)/3);

	parts[2].ymax = parts[5].ymax = parts[8].ymax = ((2*z->height)/3);
	parts[2].ymax = parts[5].ymax = parts[8].ymax = 2*z->height-1;


	for (i = 0; i < partnum; i++) {
		printf(
		generate_region(z,&parts[i]);
	}

	// Set wall character
	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			if (z->tiles[x][y].type & TILE_WALL) {
				z->tiles[x][y].ch = ACS_TTEE;
			}
		}
	}
}





// this function is really ugly
static void generate2(zone * z)
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

	z->tiles[x][y].ch = ch;
	disp_put(x, y, ch);
}

void zone_draw(zone * z)
{
	int i, j;

	wclear(dispscr);

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			disp_put(i, j, z->tiles[i][j].ch);
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
