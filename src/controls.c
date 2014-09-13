//
// controls.c
//

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "player.h"
#include "controls.h"
#include "io/input.h"
#include "io/display.h"

// Some wrapper functions called from the controls structure
static void cmd_quit(int argc, const char ** argv)
{
	disp_end();
	exit(EXIT_SUCCESS);
}

static void cmd_command(int argc, const char ** argv)
{
	if (!str_command(input_prompt_string(":"))) {
		memo("Unrecognized command.");
	}
}

static void scroll_view_center(int argc, const char ** argv)
{
	scroll_center(PLYR.x, PLYR.y);
	zone_draw(PLYR.z);
}

static void scroll_view_left(int argc, const char ** argv)
{
	scroll_disp(-1, 0);
	zone_draw(PLYR.z);
}

static void scroll_view_right(int argc, const char ** argv)
{
	scroll_disp(1, 0);
	zone_draw(PLYR.z);
}

static void scroll_view_up(int argc, const char ** argv)
{
	scroll_disp(0, -1);
	zone_draw(PLYR.z);
}

static void scroll_view_down(int argc, const char ** argv)
{
	scroll_disp(0, 1);
	zone_draw(PLYR.z);
}

static void cmd_display_controls(int argc, const char ** argv)
{
	input_display_controls();
}

// The big controls structure
// To add a control, add a line here at the same place as in the enum in controls.h
control_t controls[TOTAL_CONTROLS] = {
	// movement controls
	{ 'k', "move-up",     "Move up",         plyr_act_move_up        },
	{ 'j', "move-down",   "Move down",       plyr_act_move_down      },
	{ 'h', "move-left",   "Move left",       plyr_act_move_left      },
	{ 'l', "move-right",  "Move right",      plyr_act_move_right     },
	{ 'y', "move-uleft",  "Move up-left",    plyr_act_move_upleft    },
	{ 'u', "move-uright", "Move up-right",   plyr_act_move_upright   },
	{ 'b', "move-dleft",  "Move down-left",  plyr_act_move_downleft  },
	{ 'n', "move-dright", "Move down-right", plyr_act_move_downright },
	{ ';', "use",         "Use",             plyr_act_use            },

	// scrolling
	{ 'C', "scroll-center", "Center scrolling", scroll_view_center },
	{ 0,   "scroll-up",     "Scroll up",        scroll_view_up     },
	{ 0,   "scroll-down",   "Scroll down",      scroll_view_down   },
	{ 0,   "scroll-left",   "Scroll left",      scroll_view_left   },
	{ 0,   "scroll-right",  "Scroll right",     scroll_view_right  },

	// actions
	{ 'i', "inventory", "Show inventory",   plyr_act_inv      },
	{ 'E', "equipment", "Show equipment",   plyr_act_equipped },
	{ ',', "pickup",    "Pick up item",     plyr_act_pickup   },
	{ '.', "drop",      "Drop item",        plyr_act_drop     },
	{ 'c', "consume",   "Consume item",     plyr_act_consume  },
	{ 'w', "equip",     "Equip item",       plyr_act_equip    },
	{ 't', "throw",     "Throw item",       plyr_act_throw    },

	// stancing
	{ '1', "stance-neutral", "Stance neutral", plyr_stance_neutral },
	{ '2', "stance-defend",  "Stance defend",  plyr_stance_defense },
	{ '3', "stance-attack",  "Stance attack",  plyr_stance_attack  },

	// Miscellanious
	{ ' ', "idle",      "Idle",             plyr_act_idle        },
	{ 'o', "disp-ctrl", "Display controls", cmd_display_controls },
	{ 'Q', "quit",      "Quit",             cmd_quit             },
	{ ':', "command",   "Command prompt",   cmd_command          },
};

// Some functions from managing the commands and controls
int ctrl_by_field(const char * name)
{
	int i;
	for (i = 0; i < TOTAL_CONTROLS; i++) {
		if (!strcmp(name, controls[i].field)) return i;
	}
	return CTRL_INVALID;
}

int ctrl_by_key(int key)
{
	int i;
	for (i = 0;i < TOTAL_CONTROLS; i++) {
		if (controls[i].key == key) return i;
	}
	return CTRL_INVALID;
}

int execute(int c, int argc, const char ** argv)
{
	if (c >= 0 && c < TOTAL_CONTROLS) {
		controls[c].func(argc, argv);
		return 1;
	}
	return 0;
}

int str_command(const char * str)
{
	int ctrl, argc = 0;
	char ** argv = NULL;
	const char * start;

	// parse the command string
	while (*str) {
		while (isspace(*str)) str++;

		start = str;
		while (*str && !isspace(*str)) str++;

		if (start != str) {
			argv = realloc(argv, (argc + 1) * sizeof(char *));
			argv[argc] = malloc(str - start + 1);
			memcpy(argv[argc], start, str - start);
			argv[argc][str - start] = 0;
			argc++;
		}
	}

	// temporary
	info("Command parsed to %d arguments:", argc);
	int i;
	for (i = 0; i < argc; i++) {
		info("  argv[%d] = %s", i, argv[i]);
	}

	// execute the command
	if (argc) {
		ctrl = ctrl_by_field(argv[0]);
		return execute(ctrl, argc-1, (const char **)argv+1);
	}

	return 0;
}

int key_command(int key)
{
	return execute(ctrl_by_key(key), 0, NULL);
}
