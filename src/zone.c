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
#include "tile_object.h"

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

void set_wall_char(zone * z, int x, int y)
{
	int i, j;
	unsigned char w = 0;

	for (j = 1; j >= -1; j--) {
		for (i = 1; i >= -1; i--) {
			if (i != 0 || j != 0) {
				w <<= 1;
				w |= !!on( x + i, y + j, z);
			}
		}
	}

	z->tiles[x][y].tile = wall_chars[w];
	z->tiles[x][y].show_tile = z->tiles[x][y].tile;
}


#define sign(x) (x==0?0:(x>0?1:-1))

// returns 1 or 2 if the door should be a door, 0 otherwise
// return 2 if horizontal door, 1 if vertical door, so the retval-1 can be passed to make_door
// Might want to make this more intuitive in the future
int door_good(zone * z, int x, int y)
{
	int l = (x - 1 < 0) ? 1 : z->tiles[x - 1][y].impassible;
	int t = (y - 1 < 0) ? 1 : z->tiles[x][y - 1].impassible;
	int r = (x + 1 >= z->width ) ? 1 : z->tiles[x + 1][y].impassible;
	int b = (y + 1 >= z->height) ? 1 : z->tiles[x][y + 1].impassible;
	if ( l &&  r && !t && !b) return 2;
	if (!l && !r &&  t &&  b) return 1;
	return 0;
}

// this function is really ugly
static void generate(zone * z)
{
	int i, x, y, max, timeout;
	int wall, door_type;
	int x1,x2,y1,y2;
	int rc;
	room ** rooms;
	item * it;
	creature * cr;

	z->name = place_name(world.eth);

	rc = random() % ((z->width * z->height) / ROOM_INFREQ) + ROOM_MIN;
	rooms = malloc(sizeof(room*) * rc);

	//make everything walls!
	//TODO safely initalize zones this way
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
	//this is getting worse over time...
	for(i=0; i< rc; i++){
		max = random() % rc;
		room_spot(z,rooms[i],&x1,&y1);
		room_spot(z,rooms[max],&x2,&y2);
		wall=0;

		if(x1 == -1 || x2 == -1){
			warning("Room generation timed out!");
			continue;
		}

		for(;x1 != x2; x1-= sign(x1-x2)){
			if(z->tiles[x1][y1].impassible && !wall && (door_type = door_good(z, x1, y1))){
				z->tiles[x1][y1].obj = make_door(0, door_type - 1);
				zone_update(z,x1 ,y1);
				wall=1;
				continue;
			}

			if(z->tiles[x1][y1].impassible)
				 wall=1;
			else if (wall && (door_type = door_good(z, x1 + sign(x1-x2), y1))){
				z->tiles[x1 + sign(x1-x2)][y1].obj = make_door(0, door_type - 1);
				z->tiles[x1 + sign(x1-x2)][y1].impassible = 1;
				zone_update(z,x1 + sign(x1-x2) ,y1);
				break;
			}

				zone_empty_tile(z,x1,y1);
		}

		for(;y1 != y2; y1-= sign(y1-y2)){
			if(z->tiles[x1][y1].impassible && !wall && (door_type = door_good(z, x1, y1))){
				z->tiles[x1][y1].obj = make_door(0, door_type - 1);
				zone_update(z,x1 ,y1);
				wall=1;
				continue;
			}

			if(z->tiles[x1][y1].impassible)
				 wall=1;
			else if (wall && (door_type = door_good(z, x1, y1 + sign(y1-y2)))){
				z->tiles[x1][y1 + sign(y1-y2)].obj = make_door(0, door_type - 1);
				z->tiles[x1][y1 + sign(y1-y2)].impassible = 1;
				zone_update(z,x1 ,y1 + sign(y1-y2));
				break;
			}

			zone_empty_tile(z,x1,y1);
		}
	}

	//do rendering thing
	for (x = 0; x < z->width; x++) {
	for (y = 0; y < z->height; y++) {
		if (z->tiles[x][y].impassible && !z->tiles[x][y].obj) {
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
            debug("Generated %s at %s@%d,%d", it->name, zone_name(z), x, y);
			item_tele(it, x, y, z);
			zone_update(z, x, y);
		} else {
            warning("Failed to generate item %s (could not place in %s)", it->name, zone_name(z));
			item_free(it);
		}
	}

	// place some more random junk
	if (!config.all_alone) {
		max = random() % (z->width * z->height / CRTR_INFREQ) + CRTR_MIN;
		for (i = max; i >= 0; i--) {
			cr = gen_crtr(world.gcrtrs, 1);
			if(!crtr_spawn(cr, z)) {
                warning("Failed to spawn %s (no room in %s)", crtr_name(cr), zone_name(z));
                crtr_free(cr); //Screw it
			}
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


		z->tiles[x][y].obj = make_stair();
		zone_update(z, x, y);
	}

	for (i = 0; i < 4; i++) {
		do {
			x = random() % z->width;
			y = random() % z->height;
			timeout--;
		} while (z->tiles[x][y].impassible);


		z->tiles[x][y].obj = make_smelter();
		zone_update(z, x, y);
	}

	for (i = 0; i < 4; i++) {
		do {
			x = random() % z->width;
			y = random() % z->height;
			timeout--;
		} while (z->tiles[x][y].impassible);


		z->tiles[x][y].obj = make_forge();
		zone_update(z, x, y);
	}

}

zone * zone_new(int w, int h)
{
	int i, j;
	zone * z;
	static int first = 1;

	z = malloc(sizeof(zone));
	z->width = w;
	z->height = h;
	z->name = NULL;

	z->tiles = malloc(sizeof(tile *) * w);
	for (i = 0; i < w; i++) {
		z->tiles[i] = malloc(sizeof(tile) * h);
		for (j = 0; j < h; j++) {
			z->tiles[i][j].show = 0;
			z->tiles[i][j].impassible = 0;
			z->tiles[i][j].crtr = NULL;
			z->tiles[i][j].obj = NULL;
			z->tiles[i][j].inv = inv_new(TILE_MAX_WEIGHT);
		}
	}

	vector_init(&z->crtrs);

	if (first) {
		fill_walls();
		first = 0;
	}

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
			//free tile objects
			if(z->tiles[i][j].obj) free(z->tiles[i][j].obj);
		}
		free(z->tiles[i]);
	}

	if(z->name) free(z->name);
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
	if (c->z == PLYR.z) disp_refresh();

	c->z = NULL;
	crtr_free(c);
}

void zone_draw_tile(zone * z, int x, int y)
{
	if (z->tiles[x][y].show == 1 || config.show_all
		|| (z->tiles[x][y].show && z->tiles[x][y].impassible)
	) {
		disp_put(x, y, z->tiles[x][y].show_tile);
	} else {
		disp_put(x, y, TILE_BLANK);
	}
}

void zone_update(zone * z, int x, int y)
{
	int i;
	int weight = -1;
	item * it;
	int tile = z->tiles[x][y].tile;

	if (z->tiles[x][y].crtr != NULL && z->tiles[x][y].crtr->health > 0) {
		tile = z->tiles[x][y].crtr->tile;
	} else if(z->tiles[x][y].obj) {
		tile = z->tiles[x][y].obj->tile;
	} else {
		for (i = 0; i < z->tiles[x][y].inv->size; i++) {
			it = z->tiles[x][y].inv->itms[i];

			if (it != NULL && it->weight > weight) {
				weight = it->weight;
				tile = it->tile;
			}
		}
	}


	z->tiles[x][y].show_tile = tile;
	zone_draw_tile(z, x, y);
}

void zone_draw(zone * z)
{
	int i, j;

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			zone_draw_tile(z, i, j);
		}
	}

	disp_refresh();
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
	z->tiles[x][y].tile = TILE_FLOOR;
	z->tiles[x][y].show_tile = TILE_FLOOR;

}

const char *zone_name(zone *z)
{
	if (z->name != NULL) {
		return z->name;
	}
	return "(null zone)";
}

