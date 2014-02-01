//
// zone.c
//

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "item.h"
#include "zone.h"
#include "world.h"
#include "display.h"
#include "inventory.h"
#include "log.h"
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
	int xmin,ymin,xmax,ymax;
} partition;

typedef struct {
	int x,y;
} point_t;

typedef struct {
	double radius, angle;
} polar_t;

typedef struct {
	polar_t polar;
	point_t cart;
} ptpair_t;

// Unglobal this
point_t avg;

static void create_room_map(int *** room_map, int width, int height)
{
	int i;
	if (NULL != *room_map) wrlog("Warning: create_room_map called with non-null pointer");

	*room_map = (int**) calloc(1,sizeof(int*) * width);
	for (i = 0; i < width; i++)
		(*room_map)[i] = (int*) calloc(1,sizeof(int) * height);

}

static void clear_room_map(int ** room_map, int width, int height)
{
	int i;

	for (i = 0; i < width; i++)
		memset(room_map[i],0,sizeof(int) * height);

}

static void free_room_map(int ** room_map, int width)
{
	int i;

	for (i = 0; i < width; i++)
		free(room_map[i]);
	free(room_map);
}


static int on(zone * z, int x, int y)
{
	if (x < 0) return 1;
	if (y < 0) return 1;
	if (x >= z->width)  return 1;
	if (y >= z->height) return 1;
	return z->tiles[x][y].type & TILE_WALL;
}

static void set_wall_char(zone * z, int x, int y)
{
	int i, j;
	unsigned char ch = 0;

	for (j = 1; j >= -1; j--) {
		for (i = 1; i >= -1; i--) {
			if (i != 0 || j != 0) {
				ch <<= 1;
				ch |= !!on(z, x + i, y + j);
			}
		}
	}

	z->tiles[x][y].ch = wall_chars[ch];
}

static point_t get_center(point_t *pts, int num)
{
	int i;
	point_t ret;
	point_t sum = {0};
	

	for (i = 0; i < num; i++) {
		sum.x += pts[i].x;
		sum.y += pts[i].y;
	}

	// Centerpoint for the radial sort
	ret.x = sum.x / num;	
	ret.y = sum.y / num;		

	return ret;
}

static polar_t cart_to_polar(point_t *cart)
{
	polar_t ret;
	
	ret.radius = sqrt((cart->x * cart->x) + (cart->y * cart->y));
	ret.angle = atan2( cart->y, cart->x);

//	if (cart->x < 0) ret.angle += M_PI;
//	else if (cart->y < 0) ret.angle = (2*M_PI) + ret.angle;

	return ret;
}

// OPTIMIZE: Please. Unecessarily memory intensive
static void do_sort(point_t *pts, int num)
{
	int i,j;
	int c;

	ptpair_t *pairs = (ptpair_t*) malloc(sizeof(ptpair_t) * num);
	ptpair_t *sorted_pairs = (ptpair_t*) malloc(sizeof(ptpair_t) * num);

	// Generate pair array
	for (i = 0; i < num; i++) {
		pairs[i].cart = pts[i];
		pairs[i].polar = cart_to_polar(&pts[i]);
	}
	
	
	for (i = 0; i < num; i++) {
		c = 0;
		for (j = 0; j < num; j++) {
			if (pairs[i].polar.angle > pairs[j].polar.angle)
				c++;
		}
		sorted_pairs[c] = pairs[i];
	}

	for (i = 0; i < num; i++) {
		pts[i] = sorted_pairs[i].cart;
	}


	free(pairs);
	free(sorted_pairs);
}


static void radial_sort(point_t * pts, int num)
{
	int i;
	point_t sum = {0};

	// NOTE: Temporarily global
	//point_t avg;


	for (i = 0; i < num; i++) {
		sum.x += pts[i].x;
		sum.y += pts[i].y;
	}

	// Centerpoint for the radial sort
	avg.x = sum.x / num;	
	avg.y = sum.y / num;		


	// Change to the new center
	for (i = 0; i < num; i++) {
		pts[i].x -= avg.x;
		pts[i].y -= avg.y;
	}

	// Sort by angle now
	do_sort(pts,num);

	// Reset the points back to being relative to partition
	for (i = 0; i < num; i++) {
		pts[i].x += avg.x;
		pts[i].y += avg.y;
	}

}

static int prospect_tile(zone * z, partition * p, int ** part_map, int x, int y)
{
	int i,j;
	int ret = 1;

	if ((x >= p->xmax) || ( x < p->xmin)) return 0;
	if ((y >= p->ymax) || ( y < p->ymin)) return 0;

	if (z->tiles[x][y].type != TILE_WALL) return 1;
	if (part_map[x][y]) return 1;
	
	part_map[x][y] = 1;

	for (j = -1; j <= 1; j++) {
		for (i = -1; i <= 1; i++) {
			if ((!i) && (!j))
				continue;
			ret = ret && prospect_tile(z,p,part_map,x+i,y+j);
		}
	}

	return ret;
}



static void dig_tile(zone * z, partition * p, int x, int y)
{
	//wrlog("entered on tile (%d,%d)",x,y);
	int i,j,edge = 0;
	// Base Cases
	if ((x >= p->xmax) || ( x < p->xmin)) return;
	if ((y >= p->ymax) || ( y < p->ymin)) return;
	if (z->tiles[x][y].type == TILE_FLOOR) return;
	if (z->tiles[x][y].type == TILE_EDGE) edge = 1; //return;
		
	//wrlog("Landed on non-floor! digging...");
	z->tiles[x][y].type = TILE_FLOOR;
	z->tiles[x][y].ch = '.';

	
	for (j = -1; j <= 1; j++) {
		for (i = -1; i <= 1; i++) {
			if ((!i) && (!j))
				continue;
			if ((x+i < 0) || (x+i >= z->width)) continue;
			if ((y+j < 0) || (y+j >= z->height)) continue;
			if ((!edge) || (z->tiles[x+i][y+j].type == TILE_EDGE))
				dig_tile(z,p,x+i,y+j);
		}
	}


}

static int dig_room(zone * z, partition * p, point_t *pts, int num)
{
	int i,j;	
	int del = -2, prev_del = 2;
	int dir, prev_dir = 2; // Toggles between x and y (0 and 1)

	int ret = 0;
	int pros;
	int ** room_map = NULL;
	int cur[2];

	// Dig the border
//	dir = 0;
	dir = rand() % 2;
	for (i = 0; i < num; i++) {
		// Walk to the next point	
		memcpy(cur,&pts[i],sizeof(int) * 2);
		for (j = 0; j < 2; j++) {
			prev_del = del;
			del = ((((int*) &pts[i])[dir] < ((int*) &pts[(i+1) % num])[dir]) << 1 ) - 1; 
//			tar = abs( ((int*) &pts[(i+1) % num])[dir] - ((int*) &pts[i])[dir] );

			if ((prev_dir == dir) && (prev_del == (-del))) {
				dir = !dir;
				del = ((((int*) &pts[i])[dir] < ((int*) &pts[(i+1) % num])[dir]) << 1 ) - 1; 
			}
			zone_draw(z); // More debug
			for (; cur[dir] - ((int*) &pts[(i+1) % num])[dir] != 0; cur[dir] += del) {
				// Set each of these zone tiles to floor
				z->tiles[cur[0]][cur[1]].type = TILE_EDGE;
				z->tiles[cur[0]][cur[1]].ch = '+'; // DEBUG PORPOISES
			}

			prev_dir = dir;	
			if (!j) {
				dir = !dir;
			}
		}
	}
	// Fill the room
	point_t center = get_center(pts,num);


	create_room_map(&room_map,z->width,z->height);

	for (i = 0; i < 3; i++) {
		// TODO: Check bounds
		center = get_center(pts,num);
		center.x += rand() % 20 - 10;
		center.y += rand() % 20 - 10;

		ret |= (pros = prospect_tile(z,p,room_map,center.x,center.y));
		if (pros)
			dig_tile(z,p,center.x,center.y);

		clear_room_map(room_map,z->width,z->height);
	}

	free_room_map(room_map,z->width);
	return !ret;

	return 0;
}



// Generate a random region in the given boundary
static void generate_region(zone * z, partition * p)
{
	int i;
	// Number of pairs of points
	int num_pts = 8;
	int dir;
	int tries = 3;

	point_t diff;

	diff.x = p->xmax - p->xmin - 2;
	diff.y = p->ymax - p->ymin - 2;

	point_t *pts = (point_t*) malloc(sizeof(point_t) * num_pts);

build_room:

	// Generate a list of "paired" points
	for (i = 0; i < num_pts; i+=2) {
		dir = rand() % 2;
		pts[i].x = (rand() % diff.x) + 1;
		pts[i].y = (rand() % diff.y) + 1;
	
		// Keep one axis constant	
		((int*) &pts[i+1])[!dir] = ((int*) &pts[i])[!dir];
		// Vary the other
		((int*) &pts[i+1])[dir] = ((((int*) &pts[i])[dir]) + (rand() % ((int*) &diff)[dir])) % ((int*) &diff)[dir];

	}

	radial_sort(pts,num_pts);

	for (i = 0; i < num_pts; i++) {
		pts[i].x += p->xmin;
		pts[i].y += p->ymin;
	}

	for (i = 0; i < num_pts; i++) {
		z->tiles[pts[i].x][pts[i].y].type = TILE_FLOOR;
		z->tiles[pts[i].x][pts[i].y].ch = '.';
	}

//	z->tiles[avg.x + p->xmin][avg.y + p->ymin].type = 0x0;
//	z->tiles[avg.x + p->xmin][avg.y + p->ymin].ch = '?';

	if (dig_room(z,p,pts,num_pts)) {
		wrlog("Bad region detected, rejecting, %d tries left", tries-1);
		if (--tries)
			goto build_room;
	}

	free(pts);
}





static void crawl_room(zone * z, int ** room_map, int value, int x, int y)
{
	int i,j;
	if ((x < 0) || ( x >= z->width)) return;
	if ((y < 0) || ( y >= z->height)) return;
	if (z->tiles[x][y].type == TILE_WALL) return;
	if (room_map[x][y]) return;

	room_map[x][y] = value;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if ((!i) && (!j)) continue;
			if ((x+i < 0) || (x+i >= z->width)) continue;
			if ((y+j < 0) || (y+j >= z->height)) continue;
			crawl_room(z,room_map,value,x+j,y+i);
		}
	}



}

static void detect_rooms(zone * z, int ** room_map, int *num_rooms)
{
	int x,y;	
	*num_rooms = 1;

	for (y = 0; y < z->height; y++) {
		for (x = 0; x < z->width; x++) {
			if ((z->tiles[x][y].type == TILE_FLOOR) && (room_map[x][y] == 0))
				crawl_room(z,room_map, (*num_rooms)++, x, y);
		}
	}

	// DEBUG PORPOISES
	/*for (y = 0; y < z->height; y++) {
		for (x = 0; x < z->width; x++) {
			if (room_map[x][y] != 0) z->tiles[x][y].ch = '0'+ room_map[x][y];
		}
	}*/
}

static void generate(zone * z)
{
	int xparts,yparts;
	int i,j;
	int x,y;

	fill_walls();

	fprintf(stderr,"Bounds: %dx%d\n",z->width,z->height);

	// TODO: Randomly pick these numbers
	xparts = yparts = 3;

	// TODO: randomly select partitions (non-deterministic)	
	partition ** parts = malloc(sizeof(partition *) * xparts);
	for (i = 0; i < xparts; i++) {
		parts[i] = malloc(sizeof(partition) * yparts);
		for (j = 0; j < yparts; j++) {
			parts[i][j].xmin = (i*z->width)/3;
			parts[i][j].xmax = ((i+1)*z->width)/3;
			parts[i][j].ymin = (j*z->height)/3;
			parts[i][j].ymax = ((j+1)*z->height)/3;
		}
	}

	

	for (y = 0; y < z->height; y++) {
		for (x = 0; x < z->width; x++) {
			z->tiles[x][y].type = TILE_WALL;
		}
	}

	//partition part;
	//part.xmin = part.ymin = 0;	
	//part.xmax = z->width;
	//part.ymax = z->height;


	for (i = 0; i < xparts; i++) {
		for (j = 0; j < yparts; j++) {
			generate_region(z,&parts[i][j]);
		}
	}

	//generate_region(z,&part);
	// Set wall character
	for (x = 0; x < z->width; x++) {
		for (y = 0; y < z->height; y++) {
			if (z->tiles[x][y].type & TILE_WALL) {
				z->tiles[x][y].impassible = 1;
				set_wall_char(z,x,y);
			}
		}
	}
wrlog("getting here");
	int ** room_map = NULL;
	int num_rooms;

	create_room_map(&room_map,z->width,z->height);

	detect_rooms(z,room_map,&num_rooms);

	free_room_map(room_map,z->width);

}


// this function is really ugly
/*static void generate2(zone * z)
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
}*/

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
			z->tiles[i][j].ch = ' ';
			z->tiles[i][j].inv = inv_new(TILE_MAX_WEIGHT);
		}
	}
	fprintf(stderr,"Generating\n");
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
