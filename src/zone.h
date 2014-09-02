//
// zone.h
//

#ifndef ZONE_H
#define ZONE_H

typedef struct tile tile;
typedef struct zone zone;

#define TILE_MAX_WEIGHT 50000

#include "util.h"
#include "creature.h"
#include "inventory.h"

struct tile {
	chtype ch, show_ch;
	int show;
	int impassible;  // whether the tile is pasible or not
	creature * crtr; // creture that may be on the tile
	inventory * inv; // inventory of the tile

	int linked;
	int link_x, link_y;
	zone * link_z;
};

struct zone {
	char * name;
	int width, height; // width and height of zone
	tile ** tiles;     // array of tiles in the zone
	vector_t crtrs;    // list of all creatures in the zone
};

//
// Allocates a new zone
//
zone * zone_new(int, int);

//
// Deletes a zone
//
void zone_free(zone *);

//
// Draws the tile and the given (x, y) position
//
void zone_draw_tile(zone *, int, int);

//
// Updates the character to be drawn at the location, then draws the character
//
void zone_update(zone *, int, int);

//
// Draws the entirety of the tile
//
void zone_draw(zone *);

//
// Retrieves the creature located at the specified tile
//
tile * zone_at(zone *, int, int);

//
// Removes the creature from the tile and creature list
//
void zone_rm_crtr(zone *, creature *);

//
// This function is called once every "step"
//
void zone_step(zone *, long);

//
// This function sets up an empty tile, used for zone generation
//
void zone_empty_tile(zone *, int, int);

//
// Takes a two (x, y) pairs and a max distance and checks if the tiles are
//  within visible range of each other.
//
int zone_can_see(zone *, int, int, int, int, int);

//
// Returns the tile of a given creature
//
#define tileof(O) (&(O)->z->tiles[(O)->x][(O)->y])

//
// Safely returns the name of a zone.
//
const char *zone_name(zone *);

#endif
