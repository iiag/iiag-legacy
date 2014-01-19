//
// main.c
//

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "item.h"
#include "world.h"
#include "display.h"
#include "creature.h"
#include "inventory.h"

#define PLYRT (PLYR.z->tiles[PLYR.x][PLYR.y])

static void pickup(void)
{
	int i, j;

	if (PLYRT.inv->weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		i = inv_prompt("Pick up what?", PLYRT.inv);

		if (PLYRT.inv->size > i && PLYRT.inv->itms[i] != NULL) {
			if ((j = inv_add(PLYR.inv, PLYRT.inv->itms[i])) != INVALID) {
				PLYRT.inv->itms[i]->i = j;
				inv_rm(PLYRT.inv, i);
				memo("You pick up the %s", PLYR.inv->itms[j]->f->name);
			} else {
				memo("You cannot pick up the %s.", PLYRT.inv->itms[i]->f->name);
			}
		} else {
			memo("There is no such item.");
		}

		wclear(dispscr);
		zone_draw(PLYR.z);
		wrefresh(dispscr);
	}
}

static void drop(void)
{
	int i,j;

	i = inv_prompt("You dropped what?", PLYR.inv);

	if(PLYR.inv->size > i && PLYR.inv->itms[i]!=NULL){
		if((j=inv_add(PLYRT.inv,PLYR.inv->itms[i]))!=INVALID){
			PLYR.inv->itms[i]->i=j;
			inv_rm(PLYR.inv,i);
			memo("You Dropped %s",PLYRT.inv->itms[j]->f->name);
		}else{
			memo("You cannot drop the %s.",PLYR.inv->itms[i]->f->name);
		}
	}else{
		memo("There is no such item.");
	}

	wclear(dispscr);
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

static void show_inv(void)
{
	inv_prompt("Your inventory:", PLYR.inv);

	wclear(dispscr);
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

static void step(void)
{
	if (PLYR.health <= 0) {
		memo("You are dead.");
		wgetch(memoscr);
		end_disp();
		exit(0);
	}
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
		case 'h': crtr_move(&PLYR, -1,  0); break;
		case 'j': crtr_move(&PLYR,  0,  1); break;
		case 'k': crtr_move(&PLYR,  0, -1); break;
		case 'l': crtr_move(&PLYR,  1,  0); break;
		case 'y': crtr_move(&PLYR, -1, -1); break;
		case 'u': crtr_move(&PLYR,  1, -1); break;
		case 'b': crtr_move(&PLYR, -1,  1); break;
		case 'n': crtr_move(&PLYR,  1,  1); break;
		case 'i': show_inv(); break;
		case ',': pickup(); break;
		case '.': drop(); break;
		case 'q': goto cleanup;
		default:
			memo("Unknown key press: %c (%d)", c, c);
		}

		step();
	}

cleanup:
	end_disp();
	return 0;
}
