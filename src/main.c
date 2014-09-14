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
#include "world.h"
#include "config.h"
#include "player.h"
#include "creature.h"
#include "inventory.h"
#include "io/input.h"
#include "io/display.h"
#include "lua/lua.h"
#include "controls.h"
#include "net/net.h"
#include "net/packet.h"
#include "introspection.h"
#include "recipe.h"

static void step(void)
{
	step_world();
	update_status();
}

static void sig_handler(int rc)
{
	disp_end();
	fprintf(stderr, "\nSignal %d caught.\n", rc);
	exit(rc);
}

int main(int argc, char ** argv)
{
	int c;

	init_introspection(argv[0]);
	init_config(argc, argv);
	init_recipes();
	init_lua();

	info("loaded %i recipes",recipes.cnt);
	info("loaded %i types",item_types.cnt);

	srandom(time(NULL));

	signal(SIGSEGV, sig_handler);
	signal(SIGINT,  sig_handler);

	init_world();

	if (config.multiplayer) {
		if (client_connect(config.ip,config.port)) {
			error("Faild to connect to server.");
			goto cleanup; //failed to connect to server
		}
		write_spawn_packet(client_socket);

		// wait for response
		usleep(600000);
		while (!read_packet(client_socket, NULL));
	}

	plyr_ev_birth();
	scroll_center(PLYR.x, PLYR.y);
	disp_clear();
	zone_draw(PLYR.z);
	update_status();
	step();

	for (;;) {
		c = input_get_ctrl();
		if(ctrl_by_key(c) != CTRL_SKIP_TURN || (!config.multiplayer))
			reset_memos();

		if (!key_command(c)) {
			memo("Unknown key press %d.\n", c);
		}

		if(c != CTRL_SKIP_TURN)
		write_command_packet(client_socket,ctrl_by_key(c));

		// TODO this delay should probably sync to game time
		if(config.multiplayer) usleep(50000);
		else if (config.real_time) usleep(250000);

		while (PLYR.act != NULL) {
			// start_timer();
			step();
			// end_timer("step length");
		}

		if (config.multiplayer) {
			while (!read_packet(client_socket, NULL));
		}
	}

	//TODO clean up world and recipes
cleanup:
	disp_end();
	return 0;
}
