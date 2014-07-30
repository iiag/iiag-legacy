//
// main.c
//

#include <lua.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include "log.h"
#include "item.h"
#include "input.h"
#include "world.h"
#include "config.h"
#include "player.h"
#include "display.h"
#include "creature.h"
#include "inventory.h"
#include "lua/lua.h"

static void update_status(void)
{
	char time[100];

	statline(0, " Hp: %d/%d | Stm: %.2f%% | Wt: %d.%d/%d.%d | Xp: %d/%d | Lvl: %d",
		PLYR.health, PLYR.max_health,
		100. * (double)PLYR.stamina / (double)PLYR.max_stamina,
		PLYR.inv->weight / 100, PLYR.inv->weight % 100,
		PLYR.inv->max_weight / 100, PLYR.inv->max_weight % 100,
		PLYR.xp, PLYR.need_xp,
		PLYR.level
	);

	statline(1, " Attack: %d | AC: %d",
		PLYR.attack,
		PLYR.ac
	);

	get_time(time, 100);
	statline(2, " %s", time);
}

static void step(void)
{
	step_world();
	update_status();
}

static void sig_handler(int rc)
{
	end_disp();
	fprintf(stderr, "\nSignal %d caught.\n", rc);
	exit(rc);
}


int main(int argc, char ** argv)
{
	int c;

	init_config(argc, argv);
	init_lua();

	srandom(time(NULL));

	signal(SIGSEGV, sig_handler);
	signal(SIGINT,  sig_handler);

	init_disp();
	init_world();

	plyr_ev_birth();
	scroll_center(PLYR.x, PLYR.y);
	zone_draw(PLYR.z);
	update_status();

	step();
	for (;;) {
		c = get_ctrl();
		reset_memos();

		switch (c) {
		// movement
		case CTRL_LEFT:   plyr_act_move(-1,  0); break;
		case CTRL_DOWN:   plyr_act_move( 0,  1); break;
		case CTRL_UP:     plyr_act_move( 0, -1); break;
		case CTRL_RIGHT:  plyr_act_move( 1,  0); break;
		case CTRL_ULEFT:  plyr_act_move(-1, -1); break;
		case CTRL_URIGHT: plyr_act_move( 1, -1); break;
		case CTRL_DLEFT:  plyr_act_move(-1,  1); break;
		case CTRL_DRIGHT: plyr_act_move( 1,  1); break;

		// scrolling
		case CTRL_SCRL_CENTER: scroll_center(PLYR.x, PLYR.y); zone_draw(PLYR.z); break;
		case CTRL_SCRL_LEFT:   scroll_disp(-1,  0);           zone_draw(PLYR.z); break;
		case CTRL_SCRL_RIGHT:  scroll_disp( 1,  0);           zone_draw(PLYR.z); break;
		case CTRL_SCRL_UP:     scroll_disp( 0, -1);           zone_draw(PLYR.z); break;
		case CTRL_SCRL_DOWN:   scroll_disp( 0,  1);           zone_draw(PLYR.z); break;

		// actions
		case CTRL_DISP_INV: plyr_act_inv();      break;
		case CTRL_DISP_EQP: plyr_act_equipped(); break;
		case CTRL_PICKUP:   plyr_act_pickup();   break;
		case CTRL_DROP:     plyr_act_drop();     break;
		case CTRL_CONSUME:  plyr_act_consume();  break;
		case CTRL_EQUIP:    plyr_act_equip();    break;
		case CTRL_THROW:    plyr_act_throw();    break;

		// miscellaneous
		case CTRL_SKIP_TURN: break;
		case CTRL_QUIT: goto cleanup;

		default:
			memo("Unknown key press");
			break;
		}

		while (PLYR.act != NULL) {
			start_timer();
			step();
			end_timer("step length");
		}
	}

cleanup:
	end_disp();
	return 0;
}
