//
// player.c
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "player.h"
#include "controls.h"
#include "tileset.h"
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


	disp_refresh();
}

// this probably should not be here
static void redraw(void)
{
	disp_clear();
	zone_draw(PLYR.z);
	disp_refresh();
}

//
// The following functions are called through the command interface
//
void plyr_act_pickup(int argc, const char ** argv)
{
	int i;

	if (PLYRT.inv->weight == 0) {
		memo("There is nothing here to pick up.");
	} else {
		i = input_prompt_inv("Pick up what?", PLYRT.inv, NULL);

		if (PLYRT.inv->size > i && PLYRT.inv->itms[i] != NULL) {
			net_inv_prompt_data=i;
			crtr_act_pickup(&PLYR, i);
		} else {
			memo("You try to pick it up, but then you realize it does not exist.");
		}

		redraw();
	}
}

void plyr_act_drop(int argc, const char ** argv)
{
	int i = input_prompt_inv("You dropped what?", PLYR.inv, &PLYR);

	if (PLYR.inv->size > i && PLYR.inv->itms[i] != NULL) {
		net_inv_prompt_data=i;
		crtr_act_drop(&PLYR, i);
	} else {
		memo("There is no such item.");
	}

	redraw();
}

void plyr_act_inv(int argc, const char ** argv)
{
	input_prompt_inv("You examine the contents of your inventory:", PLYR.inv, &PLYR);
	redraw();
}

void plyr_act_equipped(int argc, const char ** argv)
{
	input_prompt_equipped("Your equipment:", &PLYR);
	redraw();
}

void plyr_act_move_left(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, -1, 0);
}

void plyr_act_move_right(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, 1, 0);
}

void plyr_act_move_up(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, 0, -1);
}

void plyr_act_move_down(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, 0, 1);
}

void plyr_act_move_upleft(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, -1, -1);
}

void plyr_act_move_upright(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, 1, -1);
}

void plyr_act_move_downleft(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, -1, 1);
}

void plyr_act_move_downright(int argc, const char ** argv)
{
	crtr_act_aa_move(&PLYR, 1, 1);
}

void plyr_act_enter(int argc, const char ** argv)
{
	int ox, oy;
	zone * oz;
	tile * t = tileof(&PLYR);

	if (t->linked) {
		// TODO generalize

		if (t->link_z == NULL) {
			ox = PLYR.x;
			oy = PLYR.y;
			oz = PLYR.z;

			// generate new zone
			vector_append(&world.zones, zone_new(150, 50)); // TODO why 150,50?
			t->link_z = world.zones.arr[world.zones.cnt-1];

			// place player randomly
			crtr_spawn(&PLYR, t->link_z);
			t->link_x = PLYR.x;
			t->link_y = PLYR.y;

			// link back
			t = tileof(&PLYR);
			t->linked = 1;
			t->link_x = ox;
			t->link_y = oy;
			t->link_z = oz;
			t->tile = TILE_STAIRS;
			t->show_tile = TILE_STAIRS;
		} else {
			if (!crtr_tele(&PLYR, t->link_x, t->link_y, t->link_z)) {
				memo("Your way appears to be blocked?");
			}
		}

		update_vis();
		zone_draw(PLYR.z);
	} else {
		memo("I see no visible method of doing that.");
	}
}

void plyr_act_consume(int argc, const char ** argv)
{
	int i;

	i = input_prompt_inv("What dost thou consume?", PLYR.inv, &PLYR);

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

void plyr_act_throw(int argc, const char ** argv)
{
	int i, dx, dy;

	i = input_prompt_inv("Throw what?", PLYR.inv, &PLYR);
	redraw();

	if (input_prompt_dir("Throw where?", &dx, &dy)) {
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

void plyr_act_equip(int argc, const char ** argv)
{
	int i;

	i = input_prompt_inv("What dost thou equip?", PLYR.inv, &PLYR);

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

void plyr_act_cast(int argc, const char ** argv)
{
	int i, dx, dy;
	
	i = prompt_lib("Cast what?", PLYR.lib, &PLYR);
	redraw();

	if (input_prompt_dir("Cast where?", &dx, &dy)) {
		if (PLYR.lib->size > i && PLYR.lib->spls[i] != NULL) {
			crtr_act_cast(&PLYR, i, dx, dy);
		} else {
			memo("Such a spell you knoweth not!");
		}
	} else {
		memo("That is not a direction");
	}

	redraw();
}

void plyr_act_idle(int argc, const char ** argv)
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
	const char * qstr;
	int i, was_quaz = 0;

	for (i = 0; i < MAX_SLOTS; i++) {
		if (PLYR.slots[i] == NULL) continue;
		if (PLYR.slots[i]->mat == NULL) break;
		if (strcmp(PLYR.slots[i]->mat->name, "quaz") == 0) {
			was_quaz = 1;
			break;
		}
	}

	qstr = name_from_key(controls[CTRL_QUIT].key);
	if (was_quaz) {
		memo("Quaz o quaz, wherefore dost thou forsake me!? Press %s to exit.", qstr);
	} else {
		memo("You die of %s, how unfortunate. Press %s to exit.", reasons, qstr);
	}

	while (ctrl_by_key(input_get_ctrl()) != CTRL_QUIT);
	disp_end();
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
	default:;
	}
}

void plyr_stance_neutral(int argc, const char ** argv)
{
	world.plyr.stance = STANCE_NEUTRAL;
	memo("You shift your stance to a more comfortable posture.");
}

void plyr_stance_defense(int argc, const char ** argv)
{
	world.plyr.stance = STANCE_DEFENSE;
	memo("You square your shoulders, and prepare to defend yourself!");
}

void plyr_stance_attack(int argc, const char ** argv)
{
	world.plyr.stance = STANCE_ATTACK;
	memo("You lean your body forward, raise your arms, and snarl, ready to strike down all that stand before you!");
}
