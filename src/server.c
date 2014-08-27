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
#include "net/net.h"

extern command_t * command_list;
extern int num_commands;


static void sig_handler(int rc)
{
	end_disp();
	fprintf(stderr, "\nSignal %d caught.\n", rc);
	exit(rc);
}


int main(int argc, char ** argv)
{

	init_config(argc, argv);
	init_lua();

	srandom(time(NULL));

	signal(SIGSEGV, sig_handler);
	signal(SIGINT,  sig_handler);

	init_disp();
	init_world();

	/*plyr_ev_birth();
	scroll_center(PLYR.x, PLYR.y);
	zone_draw(PLYR.z);*/

	step_world();
	setup_listener(13699);

	for (;;) {
		start_timer();
		server_update_clients();
		step_world();
		end_timer("step length");
		usleep(500000);
		try_accept();
		server_listen(server_sockets);
		//if(server_sockets!=NULL)
		//write_test_packet(server_sockets->sock);


		
	}

cleanup:
	wrlog("Shutting down");
	return 0;
}