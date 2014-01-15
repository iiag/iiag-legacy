//
// zone.h
//

#ifndef ZONE_H
#define ZONE_H

struct object;

typedef struct zone {
	int width, height;
	struct object *** objs;
} zone;

zone * zone_new(int, int);
void zone_free(zone *);
void zone_update(zone *, int, int);
void zone_draw(zone *);

#endif
