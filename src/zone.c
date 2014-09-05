//
// zone.c
//

#include <math.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "log.h"
#include "item.h"
#include "walls.h"
#include "world.h"
#include "config.h"
#include "player.h"
#include "inventory.h"
#include "io/display.h"

#define ROOM_INFREQ 100
#define ITEM_INFREQ 80
#define CRTR_INFREQ 80
#define ROOM_MIN 5
#define ITEM_MIN 50
#define CRTR_MIN 40

static int on(int x, int y, zone * z)
{
	if (x < 0) return 1;
	if (y < 0) return 1;
	if (x >= z->width)  return 1;
	if (y >= z->height) return 1;
	return z->tiles[x][y].impassible;
}

static void set_wall_char(zone * z, int x, int y)
{
	int i, j;
	unsigned char ch = 0;

	for (j = 1; j >= -1; j--) {
		for (i = 1; i >= -1; i--) {
			if (i != 0 || j != 0) {
				ch <<= 1;
				ch |= !!on( x + i, y + j, z);
			}
		}
	}

	z->tiles[x][y].ch = wall_chars[ch] | COLOR_PAIR(COLOR_WALL);
	z->tiles[x][y].show_ch = z->tiles[x][y].ch;
}


#define sign(x) (x==0?0:(x>0?1:-1))

// this function is really ugly
static void generate(zone * z)
{
	static int first = 1;

	int i, x, y, max, timeout;
	int wall;
	int x1,x2,y1,y2;
	int rc;
	room ** rooms;
	item * it;
	creature * cr;

	if (first) {
		fill_walls();
		first = 0;
	}

	z->name = place_name(world.eth);

	rc = random() % ((z->width * z->height) / ROOM_INFREQ) + ROOM_MIN;
	rooms = malloc(sizeof(room*) * rc);

	//make everything walls!
	for (x = 0; x < z->width; x++) {
	for (y = 0; y < z->height; y++) {
		z->tiles[x][y].impassible = 1;
	}
	}

	//gen rooms and cut out
	for(i=0; i< rc; i++){
		rooms[i]=gen_room(world.grooms,1);
		build_room(z,rooms[i]);
	}

	//make sure rooms are connected!
	//connect each room to one other room

	for(i=0; i< rc; i++){
		max = random() % rc;
		room_spot(z,rooms[i],&x1,&y1);
		room_spot(z,rooms[max],&x2,&y2);
		wall=0;

		if(x1 == -1 || x2 == -1){
			warning("Room generation timed out!");
			break;
		}

		for(;x1 != x2; x1-= sign(x1-x2)){
			if(z->tiles[x1][y1].impassible) wall=1;
			else if (wall) break;

			zone_empty_tile(z,x1,y1);
		}

		for(;y1 != y2; y1-= sign(y1-y2)){
			if(z->tiles[x1][y1].impassible) wall=1;
			else if (wall) break;

			zone_empty_tile(z,x1,y1);
		}
	}

	//do rendering thing
	for (x = 0; x < z->width; x++) {
	for (y = 0; y < z->height; y++) {
		if (z->tiles[x][y].impassible) {
			set_wall_char(z,x,y);
		}
	}
	}

	for(i=0; i< rc; i++)
		free(rooms[i]);

	free(rooms);

	// place some random junk
	max = random() % (z->width * z->height / ITEM_INFREQ) + ITEM_MIN;
	for (i = max; i >= 0; i--) {
		it = gen_item(world.gitems, 1);

		timeout = 1000;
		do {
			x = random() % z->width;
			y = random() % z->height;
			timeout--;
		} while ((z->tiles[x][y].impassible || !inv_try(z->tiles[x][y].inv, it)) && timeout);

		if (timeout) {
			item_tele(it, x, y, z);
			zone_update(z, x, y);
		} else {
			item_free(it);
		}
	}

	// place some more random junk
	if (!config.all_alone) {
		max = random() % (z->width * z->height / CRTR_INFREQ) + CRTR_MIN;
		for (i = max; i >= 0; i--) {
			cr = gen_crtr(world.gcrtrs, 1);
			crtr_spawn(cr, z);
			zone_update(z, cr->x, cr->y);
		}
	}

	// place random zone jumpers
	for (i = 0; i < 4; i++) {
		do {
			x = random() % z->width;
			y = random() % z->height;
			timeout--;
		} while (z->tiles[x][y].impassible);

		z->tiles[x][y].linked = 1;
		z->tiles[x][y].link_z = NULL;
		z->tiles[x][y].ch = '@';
		z->tiles[x][y].show_ch = '@';
	}

	// cleanup
	//for (x = 0; x < z->width; x++) free(walls[x]);
	//free(walls);
	//free(rv);
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

	vector_init(&z->crtrs);
	if(!config.multiplayer)
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

void zone_rm_crtr(zone * z, creature * c)
{
	assert(c->z == z);
	assert(c == z->crtrs.arr[c->z_ind]);
	assert(z->crtrs.cnt > c->z_ind);

	// remove from crtrs list
	z->crtrs.arr[c->z_ind] = z->crtrs.arr[--z->crtrs.cnt];
	((creature *)z->crtrs.arr[c->z_ind])->z_ind = c->z_ind;

	// remove from tile
	assert(tileof(c)->crtr == c);
	tileof(c)->crtr = NULL;

	zone_update(c->z, c->x, c->y);
	if (c->z == PLYR.z) wrefresh(dispscr);

	c->z = NULL;
	crtr_free(c);
}

void zone_draw_tile(zone * z, int x, int y)
{
	if (z->tiles[x][y].show == 1 || config.show_all
		|| (z->tiles[x][y].show && z->tiles[x][y].impassible)
	) {
		disp_put(x, y, z->tiles[x][y].show_ch);
	} else {
		disp_put(x, y, ' ');
	}
}

void zone_update(zone * z, int x, int y)
{
	int i;
	int weight = -1;
	item * it;
	chtype ch = z->tiles[x][y].ch;

	if (z->tiles[x][y].crtr == NULL || z->tiles[x][y].crtr->health <= 0) {
		for (i = 0; i < z->tiles[x][y].inv->size; i++) {
			it = z->tiles[x][y].inv->itms[i];

			if (it != NULL && it->weight > weight) {
				weight = it->weight;
				ch = it->ch;
			}
		}
	} else {
		ch = z->tiles[x][y].crtr->ch;
	}


	z->tiles[x][y].show_ch = ch;
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

void zone_step(zone * z, long steps)
{
	int i;

	for (i = 0; i < z->crtrs.cnt; i++) {
		crtr_step(z->crtrs.arr[i], steps);
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

void zone_empty_tile(zone * z, int x, int y){

	z->tiles[x][y].impassible = 0;
	z->tiles[x][y].ch = ('.' | A_DIM);
	z->tiles[x][y].show_ch = ('.' | A_DIM);
	z->tiles[x][y].linked = 0;

}

const char *zone_name(zone *z)
{
	if (z->name != NULL) {
		return z->name;
	}
	return "(null zone)";
}

