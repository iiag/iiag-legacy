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
#include "creature.h"
#include "inventory.h"

#define PLYRT (PLYR.obj->z->tiles[PLYR.obj->x][PLYR.obj->y])

static void show_inv_h(inventory * inv)
{
	int i;

	for (i = 0; i < inv->size; i++) {
		if (inv->objs[i] != NULL) {
			wprintw(dispscr, " %c) %s\n", ind2ch(i), inv->objs[i]->f->name);
		}
	}

	wrefresh(dispscr);
}

static void pickup(void)
{
	int i, j;

	if (PLYRT.inv->weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		wmove(dispscr, 0, 0);
		wprintw(dispscr, "Pick up what?\n");

		show_inv_h(PLYRT.inv);
		i = ch2ind(wgetch(dispscr));

		if (PLYRT.inv->size > i && PLYRT.inv->objs[i] != NULL) {
			if ((j = inv_add(PLYR.obj->inv, PLYRT.inv->objs[i])) != INVALID) {
				PLYRT.inv->objs[i]->i = j;
				inv_rm(PLYRT.inv, i);
				memo("You pick up the %s", PLYR.obj->inv->objs[j]->f->name);
			} else {
				memo("You cannot pick up the %s.", PLYRT.inv->objs[i]->f->name);
			}
		} else {
			memo("There is no such item.");
		}

		wclear(dispscr);
		zone_draw(PLYR.obj->z);
		wrefresh(dispscr);
	}
}

static void drop(void){
	int i,j;

	wmove(dispscr,0,0);
	wprintw(dispscr, "You Dropped what?\n");
	show_inv_h(PLYR.obj->inv);
	i = ch2ind(wgetch(dispscr));

	if(PLYR.obj->inv->size > i && PLYR.obj->inv->objs[i]!=NULL){
		if((j=inv_add(PLYRT.inv,PLYR.obj->inv->objs[i]))!=INVALID){
			PLYR.obj->inv->objs[i]->i=j;
			inv_rm(PLYR.obj->inv,i);
			memo("You Dropped %s",PLYRT.inv->objs[j]->f->name);
		}else{
			memo("You cannot drop the %s.",PLYR.obj->inv->objs[i]->f->name);
		}
	}else{
		memo("There is no such item.");
	}

	wclear(dispscr);
	zone_draw(PLYR.obj->z);
	wrefresh(dispscr);
}

static void show_inv(void)
{
	wmove(dispscr, 0, 0);
	wprintw(dispscr, "Weight: %d/%d\n", PLYR.obj->inv->weight, PLYR.obj->inv->max_weight);

	show_inv_h(PLYR.obj->inv);
	wgetch(dispscr);

	wclear(dispscr);
	zone_draw(PLYR.obj->z);
	wrefresh(dispscr);
}

static void step(void)
{
	if (PLYR.obj->health <= 0) {
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

	zone_draw(PLYR.obj->z);

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
