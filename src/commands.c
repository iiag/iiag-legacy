#include <stdlib.h>
#include "commands.h"
#include "config.h"
#include "display.h"
#include "player.h"
#include "config.h"

command_t * command_list;
int num_commands;

static void insert(int keyval, void(*command)(int,char**), int *arraysz)
{
	if (num_commands >= *arraysz)
		if (NULL == realloc(command_list, sizeof(command_t) * ((*arraysz) <<= 2))) return; // FIXME: Handle issue here

	command_list[num_commands].command = command;
	command_list[num_commands].keyval = keyval;
	num_commands++;
}

void init_commands()
{
	int arraysz = 1;
	command_list = malloc(sizeof(command_t) * 30);
	num_commands = 0;


	// TODO: Don't hard code this!
	// TODO: Replace all functions with ones that take argc+argv
	// Movement
	insert(CTRL_LEFT,  plyr_act_move_left,  &arraysz);
	insert(CTRL_RIGHT, plyr_act_move_right, &arraysz);
	insert(CTRL_UP,    plyr_act_move_up,    &arraysz);
	insert(CTRL_DOWN,  plyr_act_move_down,  &arraysz);

	// Scrolling
	insert(CTRL_SCRL_CENTER, scroll_view_center, &arraysz);
	insert(CTRL_SCRL_LEFT,   scroll_view_left,   &arraysz);
	insert(CTRL_SCRL_RIGHT,  scroll_view_right,  &arraysz);
	insert(CTRL_SCRL_UP,     scroll_view_up,     &arraysz);
	insert(CTRL_SCRL_DOWN,   scroll_view_down,   &arraysz);
	
	// Actions
	insert(CTRL_DISP_INV,  plyr_act_inv,      &arraysz);
	insert(CTRL_DISP_EQP,  plyr_act_equipped, &arraysz);
	insert(CTRL_PICKUP,    plyr_act_pickup,   &arraysz);
	insert(CTRL_DROP,      plyr_act_drop,     &arraysz);
	insert(CTRL_CONSUME,   plyr_act_consume,  &arraysz);
	insert(CTRL_EQUIP,     plyr_act_equip,    &arraysz);
	insert(CTRL_THROW,     plyr_act_throw,    &arraysz);
	insert(CTRL_SKIP_TURN, plyr_act_idle,     &arraysz);



	// sort the array or something.

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


