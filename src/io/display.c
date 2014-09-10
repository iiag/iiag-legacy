//
// io/display.c
//

#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "../log.h"
#include "../config.h"

// so that unset functions can be ignored
static void dumb() {}

void (* graphics_end)(void) = dumb;
void (* graphics_put)(int, int, int) = dumb;
void (* graphics_dim_update)(int *, int *) = dumb;
void (* graphics_clear)  (void) = dumb;

void (* disp_refresh)(void) = dumb;

void (* memo)(const char *, ...) = (void *)dumb;
void (* reset_memos)(void) = dumb;
void (* statline)(int, const char *, ...) = (void *)dumb;

int disp_width = 0;
int disp_height = 0;
int scroll_x = 0;
int scroll_y = 0;

int ** cache = NULL;

void disp_init(void)
{
	int mode;
	FILE * f = fopen(config.tileset_file, "rb");

	if (f == NULL) {
		error("Could not open tile set file '%s'.", config.tileset_file);
		return;
	}

	if (1 != fscanf(f, " %d ", &mode)) {
		error("Could not read graphics mode from file '%s'.", config.tileset_file);
		return;
	}

	switch (mode) {
	case GR_MODE_NCURSES:
	case GR_MODE_MC_NCURSES:
		nc_init(mode, f);
		break;
	default:
		nogr_init();
		break;
	}

	fclose(f);
}

void disp_end(void)
{
	graphics_end();
	free(cache);
}

void disp_put(int x, int y, int tile)
{
	disp_dim_update();

	x -= scroll_x;
	y -= scroll_y;

	if (x >= 0 && y >= 0 && x < disp_width && y < disp_height) {
		if (cache[x][y] != tile) {
			cache[x][y] = tile;
			graphics_put(x, y, tile);
		}
	}
}

void disp_clear(void)
{
	int x;
	for (x = 0; x < disp_width; x++) {
		memset(cache[x], 0, disp_height * sizeof(int));
	}
	graphics_clear();
}

void disp_dim_update(void)
{
	int x, y, nw, nh, min;
	graphics_dim_update(&nw, &nh);

	if (nw != disp_width || nh != disp_height) {
		info("Display tile dimensions is now %d by %d.", nw, nh);

		cache = realloc(cache, nw * sizeof(int *));
		min = disp_width < nw ? disp_width : nw;

		for (x = 0; x < min; x++) {
			cache[x] = realloc(cache[x], sizeof(int) * nh);
			for (y = disp_height; y < nh; y++) {
				cache[x][y] = 0;
			}
		}

		for (; x < nw; x++) {
			cache[x] = calloc(sizeof(int), nh);
		}

		disp_width  = nw;
		disp_height = nh;
	}
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

	cx = disp_width  / 2;
	cy = disp_height / 2;

	scroll_x = x - cx;
	scroll_y = y - cy;

	if (scroll_x < 0) scroll_x = 0;
	if (scroll_y < 0) scroll_y = 0;
}

