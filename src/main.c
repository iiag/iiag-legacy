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
#include "item.h"
#include "world.h"
#include "player.h"
#include "display.h"
#include "options.h"
#include "creature.h"
#include "inventory.h"
#include "lua/lua.h"

static void update_status(void)
{
	statline(0, " Hp: %d/%d | Stm: %d/%d | Wt: %d/%d | Xp: %d/%d | Lvl: %d",
		PLYR.health, PLYR.f->max_health,
		PLYR.stamina, PLYR.f->max_stamina,
		PLYR.inv->weight, PLYR.inv->max_weight,
		PLYR.xp, PLYR.need_xp,
		PLYR.level
	);

	statline(1, " Attack: %d | AC: %d",
		PLYR.attack,
		PLYR.ac
	);
}

static void step(void)
{
	static int step = 0;

	update_status();
	assert(PLYR.health > 0);

	zone_step(PLYR.z, step);
	step = !step;
}

static void sig_handler(int rc)
{
	end_disp();
	fprintf(stderr, "Signal %d caught.\n", rc);
	exit(rc);
}

int main(int argc, char ** argv)
{
	int c;

	parse_options(argc, argv);
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

	for (;;) {
continue_loop:
		c = wgetch(memoscr);
		reset_memos();

		switch (c) {
		case 'h': plyr_act_move(-1,  0); break;
		case 'j': plyr_act_move( 0,  1); break;
		case 'k': plyr_act_move( 0, -1); break;
		case 'l': plyr_act_move( 1,  0); break;
		case 'y': plyr_act_move(-1, -1); break;
		case 'u': plyr_act_move( 1, -1); break;
		case 'b': plyr_act_move(-1,  1); break;
		case 'n': plyr_act_move( 1,  1); break;
		case 'i': plyr_act_inv();      break;
		case 'E': plyr_act_equipped(); break;
		case ',': plyr_act_pickup();   break;
		case '.': plyr_act_drop();     break;
		case 'c': plyr_act_consume();  break;
		case 'w': plyr_act_equip();    break;
		case 'C': scroll_center(PLYR.x, PLYR.y); zone_draw(PLYR.z); goto continue_loop;
		case KEY_LEFT:  scroll_disp(-1,  0); zone_draw(PLYR.z); goto continue_loop;
		case KEY_RIGHT: scroll_disp( 1,  0); zone_draw(PLYR.z); goto continue_loop;
		case KEY_UP:    scroll_disp( 0, -1); zone_draw(PLYR.z); goto continue_loop;
		case KEY_DOWN:  scroll_disp( 0,  1); zone_draw(PLYR.z); goto continue_loop;
		case ' ': break;
		case 'q': goto cleanup;
		default:
			memo("Unknown key press: %c (%d)", c, c);
			goto continue_loop;
		}

		step();
	}

cleanup:
	end_disp();
	return 0;
}
