//
// zone.h
//

#ifndef ZONE_H
#define ZONE_H

#define TILE_MAX_WEIGHT 1000

struct inventory;

typedef struct zone {
	int width, height;
	struct inventory *** tiles;
} zone;

zone * zone_new(int, int);
void zone_free(zone *);
void zone_update(zone *, int, int);
void zone_draw(zone *);

#endif
