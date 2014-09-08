//
// display.c
//

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "../world.h"
#include "../config.h"
#include "../player.h"

#define NONE (-1)

static int max_width;
static int max_height;

static int scroll_x = 0;
static int scroll_y = 0;

static int last_col = NONE;
static int inited_disp = 0;

WINDOW * memoscr;
WINDOW * dispscr;
WINDOW * statscr;

void init_disp(void)
{
	initscr();
	if(has_colors()){
		start_color();
		if(can_change_color()){
			//for the blackest of the blacks
			init_color(COLOR_BLACK, 0, 0, 0);
		}

		init_pair(COLOR_SELF, COLOR_GREEN, COLOR_BLACK);
		init_pair(COLOR_OTHER, COLOR_BLUE, COLOR_BLACK);
		init_pair(COLOR_WALL, COLOR_WHITE, COLOR_BLACK);
		//init_pair(COLOR_FLOOR, COLOR_GREY, COLOR_BLACK);
		init_pair(COLOR_ENEMY, COLOR_RED, COLOR_BLACK);
		init_pair(COLOR_ITEM, COLOR_YELLOW, COLOR_BLACK);
	}
	cbreak();
	noecho();
	getmaxyx(stdscr, max_height, max_width);

	// just hide the cursor for now
	curs_set(0);

	memoscr = newwin(1, max_width, 0, 0);
	dispscr = newwin(max_height - 4, max_width, 1, 0);
	statscr = newwin(3, max_width, max_height - 3, 0);

	if ( memoscr == NULL || dispscr == NULL || statscr == NULL ) {
		end_disp();
		fprintf(stderr, "Failed to initialize ncurses!\n");
		exit(EXIT_FAILURE);
	}

	keypad(stdscr,  TRUE);
	keypad(dispscr, TRUE);
	keypad(memoscr, TRUE);
	keypad(statscr, TRUE);

	if(config.real_time){
		nodelay(memoscr, TRUE);
	}

	inited_disp = 1;
}

void end_disp(void)
{
	if (inited_disp) {
		endwin();
	} else {
		notice("Tried to end uninitialized display.");
	}
}

void disp_put(int x, int y, chtype ch)
{
	int w, h;

	getmaxyx(dispscr, h, w);

	x -= scroll_x;
	y -= scroll_y;

	if (x >= 0 && y >= 0 && x < w && y < h) {
		mvwaddch(dispscr, y, x, ch);
	}
}

void reset_memos(void)
{
	wmove(memoscr, 0, 0);
	wclrtoeol(memoscr);
	last_col = NONE;
}

void memo(const char * fmt, ...)
{
	#ifndef SERVER
	int dummy __attribute__((unused));;
	va_list vl;
	va_start(vl, fmt);

	if (last_col != NONE) {
		mvwprintw(memoscr, 0, last_col, "...");
		wgetch(memoscr);
	}

	wmove(memoscr, 0, 0);
	wclrtoeol(memoscr);
	vw_printw(memoscr, fmt, vl);
	getyx(memoscr, dummy, last_col);
	wrefresh(memoscr);

	va_end(vl);
	#endif
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

void scroll_disp(int dx, int dy)
{
	scroll_x += dx;
	scroll_y += dy;

	if (scroll_x < 0) scroll_x = 0;
	if (scroll_y < 0) scroll_y = 0;
}

void scroll_center(int x, int y)
{
	int cx, cy;

	getmaxyx(dispscr, cy, cx);

	cx /= 2;
	cy /= 2;

	scroll_x = x - cx;
	scroll_y = y - cy;

	if (scroll_x < 0) scroll_x = 0;
	if (scroll_y < 0) scroll_y = 0;
}

//
// The following functions move the view around
//

void scroll_view_center(int argc, const char ** argv)
{
	scroll_center(PLYR.x, PLYR.y);
	zone_draw(PLYR.z);
}

void scroll_view_left(int argc, const char ** argv)
{
	scroll_disp(-1, 0);
	zone_draw(PLYR.z);
}

void scroll_view_right(int argc, const char ** argv)
{
	scroll_disp(1, 0);
	zone_draw(PLYR.z);
}

void scroll_view_up(int argc, const char ** argv)
{
	scroll_disp(0, -1);
	zone_draw(PLYR.z);
}

void scroll_view_down(int argc, const char ** argv)
{
	scroll_disp(0, 1);
	zone_draw(PLYR.z);
}
