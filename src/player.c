//
// player.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "player.h"
#include "io/input.h"
#include "io/display.h"
#include "net/packet.h"


#define PLYRT (PLYR.z->tiles[PLYR.x][PLYR.y])

void update_vis(void)
{
	int x, y, show;
	int rx, ry;

	for (x = 0; x < PLYR.z->width; x++) {
		for (y = 0; y < PLYR.z->height; y++) {
			show = zone_can_see(PLYR.z, PLYR.x, PLYR.y, x, y, PLYR.sight);
			if (show != PLYR.z->tiles[x][y].show) {
				PLYR.z->tiles[x][y].show = show || config.forget_walls ? show : 2;
				zone_draw_tile(PLYR.z, x, y);
			}
		}
	}

	// hack to show impassible squares next to visible passible spells
	for (x = 0; x < PLYR.z->width; x++) {
		for (y = 0; y < PLYR.z->height; y++) {
			if (PLYR.z->tiles[x][y].impassible) {
				for (rx = -1; rx <= 1; rx++) {
					for (ry = -1; ry <= 1; ry++) {
						if ((rx || ry) && (x + rx >= 0) && (x + rx < PLYR.z->width) && (y + ry >= 0) && (y + ry < PLYR.z->height)) {
							if (!PLYR.z->tiles[x + rx][y + ry].impassible && PLYR.z->tiles[x + rx][y + ry].show) {
								PLYR.z->tiles[x][y].show = show || config.forget_walls ? 1 : 2;
								zone_draw_tile(PLYR.z, x, y);
							}
						}
					}
				}
			}
		}
	}


	wrefresh(dispscr);
}

//
// The following functions are called through the command interface
//
void plyr_act_pickup(int argc, char ** argv)
{
	int i;

	if (PLYRT.inv->weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		i = prompt_inv("Pick up what?", PLYRT.inv, NULL);

		if (PLYRT.inv->size > i && PLYRT.inv->itms[i] != NULL) {
			net_inv_prompt_data=i;
			crtr_act_pickup(&PLYR, i);
		} else {
			memo("You try to pick it up, but then you realize it does not exist.");
		}

		redraw();
	}
}

void plyr_act_drop(int argc, char ** argv)
{
	int i = prompt_inv("You dropped what?", PLYR.inv, &PLYR);

	if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL) {
		net_inv_prompt_data=i;
		crtr_act_drop(&PLYR, i);
	} else {
		memo("There is no such item.");
	}

	redraw();
}

void plyr_act_inv(int argc, char ** argv)
{
	prompt_inv("You examine the contents of your inventory:", PLYR.inv, &PLYR);

	redraw();
}

void plyr_act_equipped(int argc, char ** argv)
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

void plyr_act_move_left(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, -1, 0);
}

void plyr_act_move_right(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, 1, 0);
}

void plyr_act_move_up(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, 0, -1);
}

void plyr_act_move_down(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, 0, 1);
}

void plyr_act_move_upleft(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, -1, -1);
}

void plyr_act_move_upright(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, 1, -1);
}

void plyr_act_move_downleft(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, -1, 1);
}

void plyr_act_move_downright(int argc, char ** argv)
{
	crtr_act_aa_move(&PLYR, 1, 1);
}

void plyr_act_use(int argc, char ** argv)
{

	int dx, dy;
	if (prompt_dir("Use what?", &dx, &dy)) {
		if(PLYR.z->tiles[PLYR.x+dx][PLYR.y+dy].obj){
			net_dir_prompt = encode_dir(dx,dy);
			if(!config.multiplayer){//let the server handle this
				crtr_act_use(&PLYR, dx, dy);
			}
		}else
			memo("There is nothing there to use.");

	} else {
		memo("That is not a direction.");
	}
	redraw();
}

void plyr_act_consume(int argc, char ** argv)
{
	int i;

	i = prompt_inv("What dost thou consume?", PLYR.inv, &PLYR);

	if(PLYR.inv->size > i && PLYR.inv->itms[i]!=NULL) {
		if (PLYR.inv->itms[i]->type & ITEM_CONSUMABLE) {
			net_inv_prompt_data=i;
			crtr_act_consume(&PLYR, i);
		} else {
			memo("That would upset thy stomach.");
		}
	} else {
		memo("Such an item existeth not.");
	}

	redraw();
}

void plyr_act_throw(int argc, char ** argv)
{
	int i, dx, dy;

	i = prompt_inv("Throw what?", PLYR.inv, &PLYR);
	redraw();

	if (prompt_dir("Throw where?", &dx, &dy)) {
		if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL) {
			net_dir_prompt = encode_dir(dx,dy);
			net_inv_prompt_data=i;
			crtr_act_throw(&PLYR, i, dx, dy);
		} else {
			memo("Such an item existeth not.");
		}
	} else {
		memo("That is not a direction.");
	}

	redraw();
}

void plyr_act_equip(int argc, char ** argv)
{
	int i;

	i = prompt_inv("What dost thou equip?", PLYR.inv, &PLYR);

	if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL){
		if (PLYR.inv->itms[i]->type & ITEM_EQUIPABLE){
			net_inv_prompt_data=i;
			crtr_act_equip(&PLYR, i);
		} else {
			memo("It seems trying to equip that would prove fruitless.");
		}
	}else{
		memo("Such an item existeth not.");
	}

	redraw();
}

void plyr_act_idle(int argc, char ** argv)
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
	int was_quaz = 0;
	//TODO reimpliment with new materials
	/*for (i = 0; i < MAX_SLOTS; i++) {
		if (PLYR.slots[i] == NULL) continue;
		if (PLYR.slots[i]->mat == NULL) break;
		if (strcmp(PLYR.slots[i]->mat->name, "quaz") == 0) {
			was_quaz = 1;
			break;
		}
	}*/

	if (was_quaz) {
		memo("Quaz o quaz, wherefore art thou forsaking me!? Press q to exit.");
	} else {
		memo("You die of %s, how unfortunate. Press q to exit.", reasons);
	}

	while (wgetch(memoscr) != 'q');
	end_disp();
	exit(0);
}

void plyr_ev_lvlup(creature * p)
{
	memo("Level up!");
	p->max_health += 5;
	p->health += 5;
	p->attack += 1;
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
		memo("That's weird... it seems that that is no longer consumable.");
		break;
	case ACT_FAIL_EQUIP_ABLE:
		memo("That's weird... it seems that that is no longer equipable.");
		break;
	case ACT_FAIL_USE:
		memo("On closer inspection, it appears useless.");
		break;
	default:;
	}
}
