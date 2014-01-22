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

static void update_status(void)
{
	statline(0, " Hp: %d/%d | Wt: %d/%d | Xp: %d/%d | Lvl: %d",
		PLYR.health, PLYR.f->max_health,
		PLYR.inv->weight, PLYR.inv->max_weight,
		PLYR.xp, PLYR.need_xp,
		PLYR.level
	);
}

static void step(void)
{
	static int step = 0;

	update_status();

	if (PLYR.health <= 0) {
		memo("You are dead.");
		wgetch(memoscr);
		end_disp();
		exit(0);
	}

	zone_step(PLYR.z, step);
	step = !step;
}

static void moveplyr(int dx, int dy)
{
	int dam;
	tile * t;

	if (!crtr_move(&PLYR, dx, dy)) {
		t = zone_at(PLYR.z, PLYR.x + dx, PLYR.y + dy);

		// auto attack
		if (t != NULL && t->crtr != NULL) {
			switch (dam = crtr_attack(&PLYR, t->crtr)) {
			case DEAD:
				memo("You kill the %s", t->crtr->f->name);
				crtr_free(t->crtr);
				t->crtr = NULL;

				zone_update(PLYR.z, PLYR.x + dx, PLYR.y + dy);
				wrefresh(dispscr);
				break;
			case 0:
				memo("You miss");
				break;
			default:
				memo("You hit for %d damage", dam);
				break;
			}
		}
	}
}

int main(int argc, char ** argv)
{
	int c;

	srand(time(NULL));

	init_disp();
	init_world();

	zone_draw(PLYR.z);
	update_status();

	for (;;) {
		c = wgetch(memoscr);
		reset_memos();

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
