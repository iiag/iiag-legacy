//
// player.c
//

#include <assert.h>
#include <stdlib.h>
#include "input.h"
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

//
// The following functions are called through the command interface
//
void plyr_act_pickup(void)
{
	int i;

	if (PLYRT.inv->weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		i = prompt_inv("Pick up what?", PLYRT.inv, NULL);

		if (PLYRT.inv->size > i && PLYRT.inv->itms[i] != NULL) {
			crtr_act_pickup(&PLYR, i);
		} else {
			memo("You try to pick it up, but then you realize it does not exist.");
		}

		redraw();
	}
}

void plyr_act_drop(void)
{
	int i = prompt_inv("You dropped what?", PLYR.inv, &PLYR);

	if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL) {
		crtr_act_drop(&PLYR, i);
	} else {
		memo("There is no such item.");
	}

	redraw();
}

void plyr_act_inv(void)
{
	prompt_inv("You examine the contents of your inventory:", PLYR.inv, &PLYR);
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
	crtr_act_aa_move(&PLYR, dx, dy);
}

void plyr_act_consume(void)
{
	int i;

	i = prompt_inv("What dost thou consume?", PLYR.inv, &PLYR);

	if(PLYR.inv->size > i && PLYR.inv->itms[i]!=NULL) {
		if (PLYR.inv->itms[i]->type & ITEM_CONSUMABLE) {
			crtr_act_consume(&PLYR, i);
		} else {
			memo("That would upset thy stomach.");
		}
	} else {
		memo("Such an item existeth not.");
	}

	redraw();
}

void plyr_act_throw(void)
{
	int i, dx, dy;

	i = prompt_inv("Throw what?", PLYR.inv, &PLYR);
	redraw();

	if (prompt_dir("Throw where?", &dx, &dy)) {
		if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL) {
			crtr_act_throw(&PLYR, i, dx, dy);
		} else {
			memo("Such an item existeth not.");
		}
	} else {
		memo("That is not a direction.");
	}

	redraw();
}

void plyr_act_equip(void)
{
	int i;

	i = prompt_inv("What dost thou equip?", PLYR.inv, &PLYR);

	if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL){
		if (PLYR.inv->itms[i]->type & ITEM_EQUIPABLE){
			crtr_act_equip(&PLYR, i);
		} else {
			memo("It seems trying to equip that would prove fruitless.");
		}
	}else{
		memo("Such an item existeth not.");
	}

	redraw();
}

void plyr_act_idle(void)
{
	crtr_act_idle(&PLYR);
}

//
// The following functions are called through the event system
//
void plyr_ev_birth(void)
{
	memo("Welcome to the world of iiag, you pathetic Guaren.");
	update_vis();
}

void plyr_ev_death(creature * p, const char * reasons)
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

void plyr_ev_act_comp(creature * p, item * it)
{
	assert(p->act != NULL);

	switch (p->act->type) {
	case ACT_MOVE:
	case ACT_AA_MOVE:
		update_vis();
		break;
	case ACT_PICKUP:
		memo("Thou dost pickup the %s.", it->name);
		break;
	case ACT_DROP:
		memo("Thou dost drop the %s.", it->name);
		break;
	case ACT_CONSUME:
		memo("Thou dost consume the %s.", it->name);
		break;
	case ACT_EQUIP:
		memo("Thou dost equip the %s to thy %s.", it->name, slot_names[it->slot]);
		break;
	default:;
	}
}

void plyr_ev_act_fail(creature * p, void * how)
{
	assert(p->act != NULL);

	switch ((action_fail)how) {
	case ACT_FAIL_PICKUP_HEAVY:
		memo("The %s is too heavy to pick up.", p->inv->itms[p->act->p.ind]->name);
		break;
	case ACT_FAIL_PICKUP_PRESENT:
	case ACT_FAIL_DROP_PRESENT:
	case ACT_FAIL_CONSUME_PRESENT:
	case ACT_FAIL_EQUIP_PRESENT:
	case ACT_FAIL_THROW:
		memo("It seems to have vanished!");
		break;
	case ACT_FAIL_DROP_HEAVY:
		memo("The %s is too heavy to drop, silly you.", p->inv->itms[p->act->p.ind]->name);
		break;
	case ACT_FAIL_CONSUME_ABLE:
		memo("Thats wierd... it seems that that is no longer consumable.");
		break;
	case ACT_FAIL_EQUIP_ABLE:
		memo("Thats wierd... it seems that that is no longer equipable.");
		break;
	default:;
	}
}
