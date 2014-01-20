//
// zone.h
//

struct tile;
struct zone;

#ifndef ZONE_H
#define ZONE_H

#define TILE_MAX_WEIGHT 1000

#include "creature.h"
#include "inventory.h"

typedef struct tile {
	int impassible;
	struct creature * crtr;
	struct inventory * inv;
} tile;

typedef struct zone {//creates azone with a width and height
	int width, height;
	tile ** tiles;
} zone;

zone * zone_new(int, int);//pointer to a new zone struct 
void zone_free(zone *);
void zone_update(zone *, int, int);
void zone_draw(zone *);
tile * zone_at(zone *, int, int);
void zone_step(zone *, int);

#define tileof(O) (&(O)->z->tiles[(O)->x][(O)->y])

#endif
