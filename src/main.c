//
// main.c
//

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include "item.h"
#include "world.h"
#include "player.h"
#include "display.h"
#include "creature.h"
#include "inventory.h"

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
	assert(PLYR.health > 0);

	zone_step(PLYR.z, step);
	step = !step;
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
		case 'h': plyr_act_move(-1,  0); break;
		case 'j': plyr_act_move( 0,  1); break;
		case 'k': plyr_act_move( 0, -1); break;
		case 'l': plyr_act_move( 1,  0); break;
		case 'y': plyr_act_move(-1, -1); break;
		case 'u': plyr_act_move( 1, -1); break;
		case 'b': plyr_act_move(-1,  1); break;
		case 'n': plyr_act_move( 1,  1); break;
		case 'i': plyr_act_inv();    break;
		case ',': plyr_act_pickup(); break;
		case '.': plyr_act_drop();   break;
		case ' ': break;
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
