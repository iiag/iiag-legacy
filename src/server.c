//
// server.c
//

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
#include "net/net.h"

static void sig_handler(int rc)
{
	fprintf(stderr, "\nSignal %d caught.\n", rc);
	exit(rc);
}

int main(int argc, char ** argv)
{

	int step;
	int stall;

	init_config(argc, argv);
	init_sol();
	init_recipes();

	srandom(time(NULL));

	signal(SIGSEGV, sig_handler);
	signal(SIGINT,  sig_handler);

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
