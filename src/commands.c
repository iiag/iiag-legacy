#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "config.h"
#include "display.h"
#include "player.h"
#include "config.h"
#include "input.h"

command_t * command_list;
int num_commands;

// NOTE: cmdstr should be a NULL-terminated string
static void insert(int keyval, char * cmdstr, void(*command)(int,char**), int *arraysz)
{
	int cmdstrlen;
/*
	if (num_commands >= *arraysz)
		if (NULL == realloc(command_list, sizeof(command_t) * ((*arraysz) <<= 2))) return; // FIXME: Handle issue here
*/
	command_list[num_commands].command = command;
	command_list[num_commands].keyval = keyval;

	cmdstrlen = strlen(cmdstr) + 1; // to include NULL

	command_list[num_commands].cmdstr = malloc(sizeof(char)*cmdstrlen);
	strncpy(command_list[num_commands].cmdstr, cmdstr, cmdstrlen); 
	num_commands++;
}

// TODO: This
static void run_command(char * cmd, int argc, char ** argv)
{
	int i;

	for (i = 0; i < num_commands; i++) {
		if (!strcmp(cmd, command_list[i].cmdstr)) {
			command_list[i].command(argc, argv);
			return;
		}
	}
	memo("Command '%s' unrecognized!", cmd);

}

void init_commands(void)
{
	int arraysz = 20;
	command_list = malloc(sizeof(command_t) * arraysz);
	num_commands = 0;


	// TODO: Don't hard code this!
	// TODO: Replace all functions with ones that take argc+argv
	// Movement
	insert(CTRL_LEFT,   "move_left",   plyr_act_move_left,       &arraysz);
	insert(CTRL_RIGHT,  "move_right",  plyr_act_move_right,      &arraysz);
	insert(CTRL_UP,     "move_up",     plyr_act_move_up,         &arraysz);
	insert(CTRL_DOWN,   "move_down",   plyr_act_move_down,       &arraysz);
	insert(CTRL_ULEFT,  "move_uleft",  plyr_act_move_upleft,     &arraysz);
	insert(CTRL_URIGHT, "move_udown",  plyr_act_move_upright,    &arraysz);
	insert(CTRL_DLEFT,  "move_dleft",  plyr_act_move_downleft,   &arraysz);
	insert(CTRL_DRIGHT, "move_dright", plyr_act_move_downright,  &arraysz);
	insert(CTRL_ENTER,  "enter",       plyr_act_enter,           &arraysz);

	// Scrolling
	insert(CTRL_SCRL_CENTER, "scroll_center", scroll_view_center, &arraysz);
	insert(CTRL_SCRL_LEFT,   "scroll_left",   scroll_view_left,   &arraysz);
	insert(CTRL_SCRL_RIGHT,  "scroll_right",  scroll_view_right,  &arraysz);
	insert(CTRL_SCRL_UP,     "scroll_up",     scroll_view_up,     &arraysz);
	insert(CTRL_SCRL_DOWN,   "scroll_down",   scroll_view_down,   &arraysz);
	
	// Actions
	insert(CTRL_DISP_INV,  "inventory", plyr_act_inv,      &arraysz);
	insert(CTRL_DISP_EQP,  "equipment", plyr_act_equipped, &arraysz);
	insert(CTRL_PICKUP,    "pickup",    plyr_act_pickup,   &arraysz);
	insert(CTRL_DROP,      "drop",      plyr_act_drop,     &arraysz);
	insert(CTRL_CONSUME,   "consume",   plyr_act_consume,  &arraysz);
	insert(CTRL_EQUIP,     "equip",     plyr_act_equip,    &arraysz);
	insert(CTRL_THROW,     "throw",     plyr_act_throw,    &arraysz);
	insert(CTRL_SKIP_TURN, "idle",      plyr_act_idle,     &arraysz);

	// sort the array or something.

}

// TODO: Fix this
void deinit_commands(void)
{
	int i;

	for (i = 0; i < num_commands; i++) {
		free(command_list[i].cmdstr);
	}

	free(command_list);

}


void execute(int keypress)
{
	int i;


	// TODO: Change this to binary search?
	for (i = 0; i < num_commands; i++) {
		if (command_list[i].keyval == keypress) {
			command_list[i].command(0, NULL);
			return;
		}
	}

	memo("Unknown key press");
	return;
}

void command_mode(void)
{
	char * string;
	int i;

	string = prompt_command();

	for (i = 0; i < strlen(string); i++) {
		if (('\n' == string[i]) || (' ' == string[i])) {
			string[i++] = '\0';
			break;
		} else if ('\0' == string[i]) {
			goto exec;
		}
	}

	// TODO: Convert space-delimited string to list of strings

exec:
	run_command(string, 0, NULL);
/*
	for (i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
*/
	free(string);
}



