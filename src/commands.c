#include <stdlib.h>
#include "commands.h"
#include "config.h"
#include "display.h"
#include "player.h"

command_t * command_list;
int num_commands;

static void insert(int keyval, void(*command)(int,char**), int *arraysz)
{
	if (num_commands >= arraysz)
		if (NULL == realloc(command_list, sizeof(command_t) * (*arraysz <<= 1))) return; // TODO: Handle issue here

	command_list[num_commands].command = command;
	command_list[num_commands].keyval = keyval;
	num_commands++;
}

void init_commands()
{
	int arraysz = 1;
	command_list = malloc(sizeof(command_t) * 5);
	num_commands = 0;

	// TODO: Don't hard code this!
	// TODO: Replace all functions with ones that take argc+argv
	insert(CTRL_DISP_INV, plyr_act_inv, &arraysz);

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


