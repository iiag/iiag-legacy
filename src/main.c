//
// main.c
//

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "world.h"
#include "object.h"
#include "display.h"
#include "inventory.h"

#define PLYRT (*(PLYR.z->tiles[PLYR.x][PLYR.y]))

static void moveplyr(int dx, int dy)
{
	obj_move(&PLYR, dx, dy);
}

static void pickup(void)
{
	int i, j;

	for (i = 0; i < PLYRT.size; i++) {
		if (PLYRT.objs[i] != NULL && &world.plyr != PLYRT.objs[i]) {
			if ((j = inv_add(PLYR.inv, PLYRT.objs[i])) != INVALID) {
				PLYRT.objs[i]->i = j;
				inv_rm(&PLYRT, i);
				memo("You pick up the %s", PLYR.inv->objs[i]->f->name);
				return;
			} else {
				memo("You cannot pick up the %s.", PLYRT.objs[i]->f->name);
			}
		}
	}

	memo("You don't pick up anything.");
}

static void show_inv(void)
{
	int i;

	wclear(dispscr);
	wmove(dispscr, 0, 0);

	wprintw(dispscr, "Weight: %d/%d\n", PLYR.inv->weight, PLYR.inv->max_weight);

	for (i = 0; i < PLYR.inv->size; i++) {
		if (PLYR.inv->objs[i] != NULL) {
			wprintw(dispscr, "~ %s\n", PLYR.inv->objs[i]->f->name);
		}
	}

	wrefresh(dispscr);
	wgetch(dispscr);

	wclear(dispscr);
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

int main(int argc, char ** argv)
{
	int c;

	srand(time(NULL));

	init_disp();
	init_world();

	zone_draw(PLYR.z);

	for (;;) {
		c = wgetch(memoscr);
		memo("");

		switch (c) {
		case 'h': moveplyr(-1,  0); break;
		case 'j': moveplyr( 0,  1); break;
		case 'k': moveplyr( 0, -1); break;
		case 'l': moveplyr( 1,  0); break;
		case 'y': moveplyr(-1, -1); break;
		case 'u': moveplyr( 1, -1); break;
		case 'b': moveplyr(-1,  1); break;
		case 'n': moveplyr( 1,  1); break;
		case 'i': show_inv(); break;
		case ',': pickup(); break;
		case 'q': goto cleanup;
		default:
			memo("Unknown key press: %c (%d)", c, c);
		}
	}

cleanup:
	end_disp();
	return 0;
}
