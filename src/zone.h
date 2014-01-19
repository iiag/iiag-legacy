//
// zone.h
//

#ifndef ZONE_H
#define ZONE_H

#define TILE_MAX_WEIGHT 1000

struct inventory;
struct tile

typedef struct zone {//creates azone with a width and height
	int width, height;
	//struct inventory *** tiles; // three dimentional array of tiles ***->x **->y *->inventory
	struct tile ** tiles 
} zone;

zone * zone_new(int, int);//pointer to a new zone struct 
void zone_free(zone *);
void zone_update(zone *, int, int);
void zone_draw(zone *);

#endif
