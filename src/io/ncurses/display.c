//
// io/ncurses/display.c
//

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <locale.h>
#include "local.h"
#include "display.h"
#include "../../config.h"

#ifndef USE_NCURSES
void nc_init(FILE * f)
{
	error("Trying to use ncurses tile set when ncurses is not supported.");
}
#else

#define NONE (-1)

static int last_col = NONE;

static size_t tileset_size;
static chtype * tileset;

WINDOW * memoscr;
WINDOW * dispscr;
WINDOW * statscr;

static void nc_end(void)
{
	endwin();
}

static void nc_put(int x, int y, int tile)
{
	chtype ch = tile < tileset_size ? tileset[tile] : ' ';

	x -= scroll_x;
	y -= scroll_y;

	if (x >= 0 && y >= 0 && x < disp_width && y < disp_height) {
		mvwaddch(dispscr, y, x, ch);
	}
}

static void nc_dim_update(void)
{
	getmaxyx(dispscr, disp_width, disp_height);
}

void nc_reset_memos(void)
{
	wmove(memoscr, 0, 0);
	wclrtoeol(memoscr);
	last_col = NONE;
}

void nc_memo(const char * fmt, ...)
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

void nc_statline(int ln, const char * fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);

	wmove(statscr, ln, 0);
	wclrtoeol(statscr);
	vw_printw(statscr, fmt, vl);
	wrefresh(statscr);

	va_end(vl);
}

void nc_refresh(void)
{
	wrefresh(dispscr);
}

void nc_clear(void)
{
	wclear(dispscr);
}

void nc_init(FILE * f)
{
	int col;
	char t;
	size_t i;

	initscr();

	// initialize colors
	if (has_colors()) {
		start_color();
		if (can_change_color()) {
			// for the blackest of the blacks
			init_color(COLOR_BLACK, 0, 0, 0);
		}

		init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
		init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
		init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
		init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
		init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(COLOR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
		init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
	}

	// some ncurses initialization stuff
	cbreak();
	noecho();

	if (config.real_time) {
		nodelay(memoscr, TRUE);
	}

	// just hide the cursor for now
	curs_set(0);

	// initialize screens
	memoscr = newwin(1, disp_width, 0, 0);
	dispscr = newwin(disp_height - 4, disp_width, 1, 0);
	statscr = newwin(3, disp_width, disp_height - 3, 0);

	if (memoscr == NULL || dispscr == NULL || statscr == NULL) {
		nc_end();
		error("Failed to initialize ncurses!");
		exit(EXIT_FAILURE);
	}

	keypad(stdscr,  TRUE);
	keypad(dispscr, TRUE);
	keypad(memoscr, TRUE);
	keypad(statscr, TRUE);

	// set function pointers
	graphics_end = nc_end;
	graphics_put = nc_put;
	graphics_dim_update = nc_dim_update;

	disp_refresh = nc_refresh;
	disp_clear   = nc_clear;
	memo         = nc_memo;
	reset_memos  = nc_reset_memos;
	statline     = nc_statline;

	input_get_ctrl        = nc_get_ctrl;
	input_prompt_dir      = nc_prompt_dir;
	input_prompt_inv      = nc_prompt_inv;
	input_prompt_equipped = nc_prompt_equipped;
	input_prompt_command  = nc_prompt_command;

	disp_dim_update();

	// read tileset from file
	if (1 != fscanf(f, " %lu", &tileset_size)) {
		error("Could not read in size of tile set.");
		tileset_size = 0;
	} else {
		tileset = malloc(tileset_size * sizeof(chtype));

		for (i = 0; i < tileset_size; i++) {
			if (1 != fscanf(f, " %c %d", &t, &col)) {
				warning("Tile set given is only of size %lu.", i);
			} else {
				tileset[i] = t | COLOR_PAIR(col);
			}
		}
	}
}

#endif // #ifndef USE_NCURSES ; #else
