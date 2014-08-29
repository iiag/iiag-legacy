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
#include <unistd.h>
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
#include "commands.h"

extern command_t * command_list;
extern int num_commands;

static void update_status(void) {
	char time[100];

	statline(0, " Hp: %d/%d | Stm: %.2f%% | Wt: %d.%d/%d.%d deben | Xp: %d/%d | Lvl: %d",
	         PLYR.health, PLYR.max_health,
	         100. * (double)PLYR.stamina / (double)PLYR.max_stamina,
	         PLYR.inv->weight / 100, PLYR.inv->weight % 100,
	         PLYR.inv->max_weight / 100, PLYR.inv->max_weight % 100,
	         PLYR.xp, PLYR.need_xp,
	         PLYR.level);

	statline(1, " Attack: %d | AC: %d | Location: %s",
	         PLYR.attack,
	         PLYR.ac,
	         PLYR.z->name);

	get_time(time, 100);
	statline(2, " %s", time);
}

static void step(void) {
	step_world();
	update_status();
}

static void sig_handler(int rc) {
	end_disp();
	fprintf(stderr, "\nSignal %d caught.\n", rc);
	exit(rc);
}

int main(int argc, char ** argv) {
	int c;

	init_config(argc, argv);
	init_lua();

	srandom(time(NULL));

	signal(SIGSEGV, sig_handler);
	signal(SIGINT,  sig_handler);

	init_disp();
	init_world();
	init_commands();

	plyr_ev_birth();
	scroll_center(PLYR.x, PLYR.y);
	zone_draw(PLYR.z);
	update_status();

	step();
	for (;;) {
		c = get_ctrl();
		reset_memos();

		if (CTRL_QUIT == c) {
			goto cleanup;
		} else if (CTRL_COMMAND == c) {
			command_mode();
		} else {
			execute(c);
		}


		// TODO this delay should probably sync to game time
		if (config.real_time) usleep(500000);

		while (PLYR.act != NULL) {
			start_timer();
			step();
			end_timer("step length");
		}
	}

cleanup:
	end_disp();
	deinit_commands();
	return 0;
}
