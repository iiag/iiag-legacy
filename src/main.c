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

static void show_inv_h(inventory * inv)
{
	int i;

	for (i = 0; i < inv->size; i++) {
		if (inv->objs[i] != NULL && inv->objs[i] != &PLYR) {
			wprintw(dispscr, " %c) %s\n", ind2ch(i), inv->objs[i]->f->name);
		}
	}

	wrefresh(dispscr);
}

static void pickup(void)
{
	int i, j;

	if (PLYRT.weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		wmove(dispscr, 0, 0);
		wprintw(dispscr, "Pick up what?\n");

		show_inv_h(&PLYRT);
		i = ch2ind(wgetch(dispscr));

		if (PLYRT.size > i && PLYRT.objs[i] != NULL && PLYRT.objs[i] != &PLYR) {
			if ((j = inv_add(PLYR.inv, PLYRT.objs[i])) != INVALID) {
				PLYRT.objs[i]->i = j;
				inv_rm(&PLYRT, i);
				memo("You pick up the %s", PLYR.inv->objs[i]->f->name);
			} else {
				memo("You cannot pick up the %s.", PLYRT.objs[i]->f->name);
			}
		} else {
			memo("There is no such item.");
		}

		wclear(dispscr);
		zone_draw(PLYR.z);
		wrefresh(dispscr);
	}
}

static void show_inv(void)
{
	wmove(dispscr, 0, 0);
	wprintw(dispscr, "Weight: %d/%d\n", PLYR.inv->weight, PLYR.inv->max_weight);

	show_inv_h(PLYR.inv);
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
