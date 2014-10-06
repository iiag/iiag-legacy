//
// server.c
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
#include "world.h"
#include "config.h"
#include "player.h"
#include "recipe.h"
#include "creature.h"
#include "inventory.h"
#include "io/input.h"
#include "io/display.h"
#include "lua/lua.h"
#include "net/net.h"

int main(int argc, char ** argv)
{

	int step;
	int stall;

	atexit(disp_end);
	init_config(argc, argv);
	init_recipes();
	init_lua();

	srandom(time(NULL));

	init_world();
	step_world();
	setup_listener(config.port);

	for (;;) {
		start_timer();
		server_update_clients();
		step = world.tm.steps;

		while(abs(world.tm.steps - step) < 140){
			stall = world.tm.steps;
			step_world();
			if(stall == world.tm.steps) break;
			debug("Step %i %i", world.tm.steps, step);
		}

		try_accept();
		server_listen(server_sockets);
		end_timer("Step length");
		usleep(250000);

	}

	return 0;
}
