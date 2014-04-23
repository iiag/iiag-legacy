//
// player.c
//

#include <stdlib.h>
#include "config.h"
#include "player.h"
#include "display.h"

#define PLYRT (PLYR.z->tiles[PLYR.x][PLYR.y])

static void update_vis(void)
{
	int x, y, show;

	for (x = 0; x < PLYR.z->width; x++) {
		for (y = 0; y < PLYR.z->height; y++) {
			show = zone_can_see(PLYR.z, PLYR.x, PLYR.y, x, y, PLYR.sight);
			if (show != PLYR.z->tiles[x][y].show) {
				PLYR.z->tiles[x][y].show = show || config.forget_walls ? show : 2;
				zone_draw_tile(PLYR.z, x, y);
			}
		}
	}

	wrefresh(dispscr);
}

// this probably should not be here
static void redraw(void)
{
	wclear(dispscr);
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

void plyr_act_pickup(void)
{
	int i, j;

	if (PLYRT.inv->weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		i = inv_prompt("Pick up what?", PLYRT.inv, NULL);

		if (PLYRT.inv->size > i && PLYRT.inv->itms[i] != NULL) {
			if ((j = inv_add(PLYR.inv, PLYRT.inv->itms[i])) != INVALID) {
				PLYRT.inv->itms[i]->i = j;
				inv_rm(PLYRT.inv, i);
				memo("You pick up the %s", PLYR.inv->itms[j]->name);
			} else {
				memo("The %s is too heavy to pick up.", PLYRT.inv->itms[i]->name);
			}
		} else {
			memo("You try to pick it up, but then you realize it does not exist.");
		}

		redraw();
	}
}

void plyr_act_drop(void)
{
	int i,j;

	i = inv_prompt("You dropped what?", PLYR.inv, &PLYR);

	if(PLYR.inv->size > i && PLYR.inv->itms[i]!=NULL){
		if((j=inv_add(PLYRT.inv,PLYR.inv->itms[i]))!=INVALID){
			crtr_rm_item(&PLYR, i)->i = j;
			memo("You dropped the %s",PLYRT.inv->itms[j]->name);
		}else{
			memo("You cannot drop the %s.",PLYR.inv->itms[i]->name);
		}
	}else{
		memo("There is no such item.");
	}

	redraw();
}

void plyr_act_inv(void)
{
	inv_prompt("You examine the contents of your inventory:", PLYR.inv, &PLYR);
	redraw();
}

void plyr_act_equipped(void)
{
	int i;

	wmove(dispscr, 0, 0);
	wprintw(dispscr, "Your equipment:\n");

	for (i = 0; i < MAX_SLOTS; i++) {
		wprintw(dispscr, " %s: ", slot_names[i]);

		if (PLYR.slots[i] == NULL) wprintw(dispscr, "nothing\n");
		else wprintw(dispscr, "%s\n", PLYR.slots[i]->name);
	}

	wgetch(dispscr);
	zone_draw(PLYR.z);
	wrefresh(dispscr);
}

void plyr_act_move(int dx, int dy)
{
	int dam;
	tile * t;
	creature * def;

	if (!crtr_move(&PLYR, dx, dy)) {
		t = zone_at(PLYR.z, PLYR.x + dx, PLYR.y + dy);

		// auto attack
		if (t != NULL && t->crtr != NULL) {
			def = t->crtr;

			switch (dam = crtr_attack(&PLYR, def)) {
			case DEAD:
				memo("You slay the %s.", crtr_name(def));
				crtr_free(def);

				zone_update(PLYR.z, PLYR.x + dx, PLYR.y + dy);
				wrefresh(dispscr);
				break;
			case 0:
				memo("You miss.");
				break;
			default:
				memo("You hit for %d damage.", dam);
				break;
			}
		}
	}

	update_vis();
}

void plyr_act_consume(void)
{
	int i;
	item * it;

	i = inv_prompt("What dost thou consume?", PLYR.inv, &PLYR);

	if(PLYR.inv->size > i && PLYR.inv->itms[i]!=NULL){
		if (PLYR.inv->itms[i]->type & ITEM_CONSUMABLE){
			it = crtr_rm_item(&PLYR, i);

			PLYR.health  += it->restore_health;
			PLYR.stamina += it->restore_stamina;

			if (PLYR.health  > PLYR.max_health ) PLYR.health  = PLYR.max_health;
			if (PLYR.stamina > PLYR.max_stamina) PLYR.stamina = PLYR.max_stamina;

			memo("Thou dost consume the %s.", it->name);
			item_free(it);
		}else{
			memo("That would upset thy stomach.");
		}
	}else{
		memo("Such an item existeth not.");
	}

	redraw();
}

void plyr_act_equip(void)
{
	int i;
	item * it;

	i = inv_prompt("What dost thou equip?", PLYR.inv, &PLYR);

	if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL){
		if (PLYR.inv->itms[i]->type & ITEM_EQUIPABLE){
			it = PLYR.inv->itms[i];
			crtr_equip(&PLYR, it, it->slot);

			memo("Thou dost equip the %s to thy %s.", it->name, slot_names[it->slot]);
		} else {
			memo("It seems trying to equip that would prove fruitless.");
		}
	}else{
		memo("Such an item existeth not.");
	}

	redraw();
}

void plyr_ev_birth(void)
{
	memo("Welcome to the world of iiag, you pathetic Guaren.");
	update_vis();
}

void plyr_ev_death(const char * reasons)
{
	memo("You die of %s, how unfortunate. Press q to exit.", reasons);
	while (wgetch(memoscr) != 'q');
	end_disp();
	exit(0);
}

void plyr_ev_lvlup(void)
{
	memo("Level up!");
	PLYR.max_health += 5;
	PLYR.health += 5;
	PLYR.attack += 1;
}
