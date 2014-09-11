//
// io/ncurses/display.c
//

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "keys.h"
#include "local.h"
#include "display.h"
#include "controlls.h"
#include "../input.h"
#include "../display.h"
#include "../../config.h"
#include "../../tileset.h"

#ifndef WITH_NCURSES
void nc_init(int mode, FILE * f)
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

	if (x >= 0 && y >= 0 && x < disp_width && y < disp_height) {
		mvwaddch(dispscr, y, x, ch);
	} else {
		warning("Out of bounds call to nc_put with coordinates %d, %d.", x, y);
	}
}

static void nc_dim_update(int * w, int * h)
{
	getmaxyx(dispscr, *h, *w);
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

// gets a named chtype from a list
static chtype special(const char * name)
{
	static int first = 1;

	// TODO static names; binary search implementation should be easy
	// TODO make constant
	static struct {
		char * name;
		chtype ch;
	} map[] = {
		{ "space", ' ' },
		{ "block", 0 },
		{ "vline", 0 },
		{ "hline", 0 },
		{ "ulcorner", 0 },
		{ "urcorner", 0 },
		{ "llcorner", 0 },
		{ "lrcorner", 0 },
		{ "ttee", 0 },
		{ "btee", 0 },
		{ "ltee", 0 },
		{ "rtee", 0 },
		{ "plus", 0 },
		{ "\\", '\\' },
		{ "degree", 0 },
		{ "inv_degree", 0 },
		{ "diamond", 0 },
		{ "plminus", 0 },
	};

	int i;

	if (first) {
		// there has to be a better way to do this
		// can't initialize staticly as ACS_* are non-constant
		map[ 1].ch = ACS_BLOCK;
		map[ 2].ch = ACS_VLINE;
		map[ 3].ch = ACS_HLINE;
		map[ 4].ch = ACS_ULCORNER;
		map[ 5].ch = ACS_URCORNER;
		map[ 6].ch = ACS_LLCORNER;
		map[ 7].ch = ACS_LRCORNER;
		map[ 8].ch = ACS_TTEE;
		map[ 9].ch = ACS_BTEE;
		map[10].ch = ACS_LTEE;
		map[11].ch = ACS_RTEE;
		map[12].ch = ACS_PLUS;
		map[14].ch = ACS_DEGREE;
		map[15].ch = ACS_DEGREE | A_REVERSE;
		map[16].ch = ACS_DIAMOND;
		map[17].ch = ACS_PLMINUS;

		first = 0;
	}

	for (i = 0; i < sizeof(map) / sizeof(*map); i++) {
		if (!strcmp(map[i].name, name)) {
			return map[i].ch;
		}
	}

	warning("Unrecoginized espaced ncurses character '%s'.", name);
	return '~';
}

void nc_init(int mode, FILE * f)
{
#define MAX_SHOW_SIZE 20

	int col;
	int dw, dh;
	size_t i, sz;
	char show[MAX_SHOW_SIZE];
	chtype c;

	initscr();

	// initialize colors
	if (mode != GR_MODE_MC_NCURSES && has_colors()) {
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

	// just hide the cursor for now
	curs_set(0);

	// initialize screens
	getmaxyx(stdscr, dh, dw);
	memoscr = newwin(1, dw, 0, 0);
	dispscr = newwin(dh - 4, dw, 1, 0);
	statscr = newwin(3, dw, dh - 3, 0);

	if (memoscr == NULL || dispscr == NULL || statscr == NULL) {
		nc_end();
		error("Failed to initialize ncurses!");
		exit(EXIT_FAILURE);
	}

	if (config.real_time) {
		nodelay(memoscr, TRUE);
	}

	keypad(stdscr,  TRUE);
	keypad(dispscr, TRUE);
	keypad(memoscr, TRUE);
	keypad(statscr, TRUE);

	// set function pointers
	graphics_end = nc_end;
	graphics_put = nc_put;
	graphics_dim_update = nc_dim_update;
	graphics_clear   = nc_clear;

	disp_refresh = nc_refresh;
	memo         = nc_memo;
	reset_memos  = nc_reset_memos;
	statline     = nc_statline;

	name_from_key = nc_name_from_key;
	key_from_name = nc_key_from_name;

	input_get_key         = nc_get_key;
	input_get_ctrl        = nc_get_ctrl;
	input_prompt_dir      = nc_prompt_dir;
	input_prompt_inv      = nc_prompt_inv;
	input_prompt_equipped = nc_prompt_equipped;
	input_prompt_string   = nc_prompt_string;

	input_display_controls = nc_display_controls;
	input_prompt_save_controls = nc_prompt_save_controls;

	disp_dim_update();

	// read tileset from file
	if (1 != fscanf(f, " %lu ", &tileset_size)) {
		error("Could not read in size of tile set.");
		tileset_size = 0;
	} else {
		tileset = malloc(tileset_size * sizeof(chtype));

		for (i = 0; i < tileset_size; i++) {
			// read in data up to the first space
			sz = 0;
			while (!isspace(c = fgetc(f)) && c != EOF) {
				show[sz++] = c;
			}
			show[sz] = 0;

			// check for premature EOF and read in color specifier
			if (c == EOF || 1 != fscanf(f, " %d ", &col)) {
				warning("Tile %d in tile set not formatted properly.", i);
			} else {
				// add the chtype to the tileset
				c = (show[0] == '\\') ? special(show + 1) : show[0];
				tileset[i] = c | (mode == GR_MODE_MC_NCURSES ? 0 : COLOR_PAIR(col));
			}
		}
	}

	for (i = 0; i < tileset_size; i++) {
		debug("tileset[%d] = %c (%x)", i, tileset[i], tileset[i]);
	}

#undef MAX_SHOW_SIZE
}

#endif // #ifndef USE_NCURSES ; #else
