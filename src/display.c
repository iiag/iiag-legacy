//
// display.c
//

#include <ctype.h>
#include <stdarg.h>
#include "world.h"
#include "display.h"

static int max_width;
static int max_height;
static int need_wait = 0;

WINDOW * memoscr;
WINDOW * dispscr;
WINDOW * statscr;

void init_disp(void)
{
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, max_height, max_width);

	// just hide the cursor for now
	curs_set(0);

	memoscr = newwin(1, max_width, 0, 0);
	dispscr = newwin(max_height - 4, max_width, 1, 0);
	statscr = newwin(3, max_width, max_height - 3, 0);
}

void end_disp(void)
{
	endwin();
}

void reset_memos(void)
{
	need_wait = 0;
}
#include "log.h"
void memo(const char * fmt, ...)
{
	int i, j;
	chtype ch;
	va_list vl;
	va_start(vl, fmt);

	if (need_wait) {
		// this is a little hacky
		for (i = max_width - 1; i >= 0; i++) {
			ch = mvwinch(memoscr, 0, i);
			if (!isspace(ch & 0xff)) break;
		}

		for (j = 0; j < 3 && j < max_width; j++) {
			wrlog("i + j = %d\n", i + j);
			mvwaddch(memoscr, 0, i + j, '.');
		}

		wgetch(memoscr);
	}

	wmove(memoscr, 0, 0);
	wclrtoeol(memoscr);
	vw_printw(memoscr, fmt, vl);
	wrefresh(memoscr);

	va_end(vl);
	need_wait = 1;
}

void statline(int ln, const char * fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);

	wmove(statscr, ln, 0);
	wclrtoeol(statscr);
	vw_printw(statscr, fmt, vl);
	wrefresh(statscr);

	va_end(vl);
}
