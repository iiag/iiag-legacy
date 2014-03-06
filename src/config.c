//
// config.c
//

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h> // for KEY_UP, KEY_DOWN, etc.
#include "log.h"
#include "config.h"

///// Default Configuration /////
config_t config = {
	NULL,
	"script/init.lua",
	
	{
		// movement controls
		'k', // up
		'j', // down
		'h', // left
		'l', // right
		'y', // up left
		'u', // up right
		'b', // down left
		'n', // down right
	
		// scrolling controls
		'C',
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,

		// action controls
		'i', // display inventory
		'E', // display equipped
		',', // pickup
		'.', // drop
		'c', // consume
		'w', // equip

		// miscellaneous controls
		' ', // skip turn
		'q', // quit
	},
};

///// Define the Configuration Language /////
static const struct {
	enum {
		STRING,
		CONTROL,
	} type;

	char * field;
	size_t offset;
} cfg_fields[] = {
	{ STRING, "lua-init", offsetof(config_t, lua_init) },

	// movement controls
	{ CONTROL, "ctrl-up",     offsetof(config_t, ctrl[CTRL_UP    ]) },
	{ CONTROL, "ctrl-down",   offsetof(config_t, ctrl[CTRL_DOWN  ]) },
	{ CONTROL, "ctrl-left",   offsetof(config_t, ctrl[CTRL_LEFT  ]) },
	{ CONTROL, "ctrl-right",  offsetof(config_t, ctrl[CTRL_RIGHT ]) },
	{ CONTROL, "ctrl-uleft",  offsetof(config_t, ctrl[CTRL_ULEFT ]) },
	{ CONTROL, "ctrl-uright", offsetof(config_t, ctrl[CTRL_URIGHT]) },
	{ CONTROL, "ctrl-dleft",  offsetof(config_t, ctrl[CTRL_DLEFT ]) },
	{ CONTROL, "ctrl-dright", offsetof(config_t, ctrl[CTRL_DRIGHT]) },

	// scrolling controls
	{ CONTROL, "ctrl-scrl-center", offsetof(config_t, ctrl[CTRL_SCRL_CENTER]) },
	{ CONTROL, "ctrl-scrl-up",     offsetof(config_t, ctrl[CTRL_SCRL_UP    ]) },
	{ CONTROL, "ctrl-scrl-down",   offsetof(config_t, ctrl[CTRL_SCRL_DOWN  ]) },
	{ CONTROL, "ctrl-scrl-left",   offsetof(config_t, ctrl[CTRL_SCRL_LEFT  ]) },
	{ CONTROL, "ctrl-scrl-right",  offsetof(config_t, ctrl[CTRL_SCRL_RIGHT ]) },
};

// prints help message
static void print_help()
{
	fprintf(stderr,
	"options:\n"
	"    -c [config file]\n"
	"        Specify the configuration file to use (- for stdin).\n"
	"    -h\n"
	"        Display this useful information.\n"
	"    -i [lua init file]\n"
	"        The initial lua script to run.\n"
	);

	exit(0);
}

// loads configuration from file
static void load_config(const char * file)
{
	FILE * f;

	if (!strcmp(file, "-")) {
		f = stdin;
	} else {
		f = fopen(file, "r");
		if (f == NULL) {
			wrlog("Could not open config file '%s'.", file);
			return;
		}
	}

	// TODO i'll get to it eventually

	if (f != stdin) fclose(f);
}

// handles command-line arguments and loads config file
void init_config(int argc, char ** argv)
{
	int i;

	// handle arguments
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'h':
				print_help();
			case 'c':
				config.cfg_file = argv[++i];
				break;
			case 'i':
				config.lua_init = argv[++i];
				break;
			}
		} else {
			wrlog("Command line argument '%s' ignored.", argv[i]);
		}
	}

	// load config file if given
	if (config.cfg_file != NULL) {
		load_config(config.cfg_file);
	}
}
