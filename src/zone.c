//
// zone.c
//

#include <stdlib.h>
#include "log.h"
#include "zone.h"
#include "world.h"
#include "object.h"
#include "display.h"

zone * zone_new(int w, int h)
{
	int i, j;
	zone * z;

	z = malloc(sizeof(zone));
	z->width = w;
	z->height = h;

	z->objs = malloc(sizeof(object **) * w);

	for (i = 0; i < w; i++) {
		z->objs[i] = malloc(sizeof(object *) * h);

		for (j = 0; j < h; j++) {
			z->objs[i][j] = NULL;
		}
	}

	return z;
}

void zone_free(zone * z)
{
	int i, j;

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			free(z->objs[i][j]);
		}
		free(z->objs[i]);
	}

	free(z->objs);
	free(z);
}

void zone_update(zone * z, int i, int j)
{
	chtype ch;

	if (z->objs[i][j] != NULL) {
		ch = z->objs[i][j]->ch;
	} else {
		ch = '.';
	}

	mvwaddch(dispscr, j, i, ch);
}

void zone_draw(zone * z)
{
	int i, j;

	for (i = 0; i < z->width; i++) {
		for (j = 0; j < z->height; j++) {
			zone_update(z, i, j);
		}
	}

	wrefresh(dispscr);
}
