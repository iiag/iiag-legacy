//
// input.c
//

#include <stdlib.h>
#include "input.h"
#include "display.h"
#include "../config.h"

#define MAX_CMD_ENTRY 100

//
// Gets a keypress and translates it into a control value
//
int get_ctrl(void)
{
	int i;
	int c = ERR;
	if (config.real_time) {
		int d = ERR;
		do {
			c = d;
			d = wgetch(memoscr);
		} while(d != ERR);
	} else c = wgetch(memoscr);

	for (i = 0; i < TOTAL_CONTROLS; i++) {
		if (config.ctrl[i] == c) return i;
	}

	if (c == ERR) return CTRL_SKIP_TURN;

	return CTRL_INVALID;
}

//
// Prompts for valid directional input
//
int prompt_dir(const char * prompt, int * dx, int * dy)
{
	*dx = *dy = 13;

	wmove(dispscr, 0, 0);
	wprintw(dispscr, "%s\n", prompt);
	wrefresh(dispscr);
	
	//get CTRL on disp screen, so that input blocks in multiplayer and realtime mode.
	int i,c;
	c = wgetch(dispscr);

	for (i = 0; i < TOTAL_CONTROLS; i++) {
		if (config.ctrl[i] == c) break;
	}

	switch (i) {
	case CTRL_LEFT:      *dx = -1; *dy = 0; break;
	case CTRL_DOWN:      *dx =  0; *dy = 1; break;
	case CTRL_UP:        *dx =  0; *dy =-1; break;
	case CTRL_RIGHT:     *dx =  1; *dy = 0; break;
	case CTRL_ULEFT:     *dx = -1; *dy =-1; break;
	case CTRL_URIGHT:    *dx =  1; *dy =-1; break;
	case CTRL_DLEFT:     *dx = -1; *dy = 1; break;
	case CTRL_DRIGHT:    *dx =  1; *dy = 1; break;
	case CTRL_SKIP_TURN: *dx =  0; *dy = 0; break;
	}

	wrefresh(dispscr);
	return *dx != 13;
}

//
// Displays the inventory with the prompt to the dispscr
// Waits for a keypress for a item selection
//
int prompt_inv(const char * prompt, inventory * inv, creature * c)
{
	int i;

	wmove(dispscr, 0, 0);
	wprintw(dispscr, "%s\n", prompt);

	for (i = 0; i < inv->size; i++) {
		if (inv->itms[i] != NULL) {
			wprintw(dispscr, " %c) %s",
				ind2ch(i),
				inv->itms[i]->name
			);

			if (c != NULL && item_equipped(inv->itms[i], c)) {
				wprintw(dispscr, " (equipped)");
			}

			wprintw(dispscr, "\n");
		}
	}

	wrefresh(dispscr);
	return ch2ind(wgetch(dispscr));
}

char * prompt_command(void)
{
	int c,i = 0;
	char * string = calloc(1,MAX_CMD_ENTRY);

	wmove(memoscr, 0, 0);
	waddch(memoscr, ':');

	while ('\n' != c) {
		string[i] = c = wgetch(memoscr);

		if ((c == 127)) { // Backspace
			if (i == 0) { wmove(memoscr, 0, 1); continue; }

			wmove(memoscr, 0, i);
			waddch(memoscr, ' ');
			wmove(memoscr, 0, i);
			string[--i] = '\0';
			wrefresh(memoscr);
			continue;
		}

		// Cancel command input
		if (c == 27) { // Escape
			wmove(memoscr, 0, 0);
			for (c = 0; c < i + 1; c++) {
				waddch(memoscr, ' ');
			}
			wrefresh(memoscr);
			free(string);
			return NULL;
		}

		waddch(memoscr, string[i++]);
		wrefresh(memoscr);
	}
	return string;
}
