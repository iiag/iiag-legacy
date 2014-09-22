//
// io/sdl/display.c
//

#include <stdio.h>
#include "../../log.h"

#ifndef WITH_SDL

void sdl_init(FILE *f) {
    error("Cannot use SDL backend (not compiled in)");
}

#else

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdarg.h>
#include <unistd.h>
#include "input.h"
#include "display.h"
#include "../input.h"
#include "../display.h"

#define TILE_RECT(x, y) {(x)*twidth, (y)*theight, twidth, theight}

SDL_Window *win;
unsigned int swidth, sheight, twidth, theight, trows, tcols;
SDL_Surface *disp;
static SDL_Surface **tiles;
static size_t sztiles;
TTF_Font *font;

const SDL_Color COL_WHITE={255, 255, 255, 255};

void sdl_put(int x, int y, int tile)
{
	SDL_Rect r = TILE_RECT(x+1, y);
	SDL_Rect ss = {0, theight, disp->w, disp->h-theight*3};
	SDL_Rect tmp;
	SDL_Surface *text;
	char buf[16];

	if(!SDL_IntersectRect(&ss, &r, &tmp)) {
        return;
	}
    if(tile >= sztiles) {
        snprintf(buf, 16, "%d", tile);
        text = TTF_RenderUTF8_Solid(font, buf, COL_WHITE);
        SDL_BlitSurface(text, NULL, disp, &r);
        SDL_FreeSurface(text);
    } else {
        SDL_BlitScaled(tiles[tile], NULL, disp, &r);
    }
}

void sdl_dim_update(int * x, int * y)
{
	*x = tcols;
	*y = trows - 2;
}

void sdl_zoom(int sx, int sy)
{
    int w, h;

    SDL_GetWindowSize(win, &w, &h);

    if(sx > 0) {
        twidth <<= sx;
    } else {
        twidth >>= -sx;
    }

    if(sy > 0) {
        theight <<= sy;
    } else {
        theight >>= -sy;
    }

    tcols = w / twidth;
    trows = h / theight;
}

void sdl_memo(const char * fmt, ...)
{
	va_list vl;
	char s[512];
	SDL_Rect r = TILE_RECT(0, 0);
	SDL_Surface *text;
	r.w = twidth * tcols;

	va_start(vl, fmt);

	vsnprintf(s, 512, fmt, vl);
	text = TTF_RenderUTF8_Solid(font, s, COL_WHITE);
	SDL_FillRect(disp, &r, 0);
	SDL_BlitSurface(text, NULL, disp, &r);
	SDL_FreeSurface(text);
	SDL_UpdateWindowSurface(win);

	va_end(vl);
}

void sdl_statline(int line, const char * fmt, ...)
{
	va_list vl;
	char s[512];
	SDL_Rect r = TILE_RECT(0, trows - 3 + line);
	SDL_Surface *text;
	r.w = twidth * tcols;

	va_start(vl, fmt);

	vsnprintf(s, 512, fmt, vl);
	text = TTF_RenderUTF8_Solid(font, s, COL_WHITE);
	SDL_FillRect(disp, &r, 0);
	SDL_BlitSurface(text, NULL, disp, &r);
	SDL_FreeSurface(text);
	SDL_UpdateWindowSurface(win);

	va_end(vl);
}

void sdl_clear(void) {
    SDL_FillRect(disp, NULL, 0);
    SDL_UpdateWindowSurface(win);
}

void sdl_refresh(void) {
    SDL_UpdateWindowSurface(win);
}

void sdl_end(void) {
    int i;

    for(i = 0; i < sztiles; i++) {
        SDL_FreeSurface(tiles[i]);
    }

    SDL_FreeSurface(disp);
    SDL_DestroyWindow(win);
}

void sdl_init(FILE *conf)
{
    char dirname[256], fname[256], oldwd[256], curwd[256];
    unsigned long fcol, bcol;
    size_t i;
    SDL_Surface *fmul, *img;

    info("Initializing SDL graphics backend...");

	graphics_put = sdl_put;
	graphics_end = sdl_end;
	graphics_dim_update = sdl_dim_update;
	graphics_clear = sdl_clear;
	graphics_zoom = sdl_zoom;
	disp_refresh = sdl_refresh;
	memo = sdl_memo;
	statline = sdl_statline;

	name_from_key  = sdl_name_from_key;
	key_from_name  = sdl_key_from_name;
	input_get_ctrl = sdl_get_ctrl;
	input_get_key  = sdl_get_key;
	input_prompt_dir = sdl_prompt_dir;
	input_prompt_inv = sdl_prompt_inv;
	input_prompt_equipped = sdl_prompt_equipped;
	input_prompt_string = sdl_prompt_string;

	if(SDL_Init(SDL_INIT_EVERYTHING) || TTF_Init()) {
        error("Could not initialize SDL: %s / ", SDL_GetError(), TTF_GetError());
        exit(EXIT_FAILURE);
	}
	atexit(SDL_Quit);

	if(!(win = SDL_CreateWindow("iiag", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN))) {
        error("Could not create a window: %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	if(!(font = TTF_OpenFont("DejaVuSansMono.ttf", 8))) {
        error("Could not open font: %s", TTF_GetError());
        exit(EXIT_FAILURE);
	}

	if(!(disp = SDL_GetWindowSurface(win))) {
        error("Could not create drawable: %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	fscanf(conf, "%s", dirname);

	fclose(conf);
	getcwd(oldwd, 256);
	debug("Current working directory: %s", oldwd);
	debug("Changing to %s...", dirname);
	if(chdir(dirname)) {
        error("Couldn't chdir to %s; aborting.", dirname);
        exit(EXIT_FAILURE);
    }
    getcwd(curwd, 256);
    debug("Current working directory: %s", curwd);

	if(!(conf = fopen("tileset", "r"))) {
        error("Could not open tileset; aborting.");
        exit(EXIT_FAILURE);
	}

	fscanf(conf, "%zd %d %d %d %d", &sztiles, &swidth, &sheight, &twidth, &theight);
	tcols = 640 / twidth;
	trows = 480 / theight;
	tiles = malloc(sztiles*sizeof(SDL_Surface *));
	if(!tiles) {
        error("Could not allocate graphics memory");
        exit(EXIT_FAILURE);
	}
	fmul = SDL_CreateRGBSurface(0, twidth, theight, 24, 0xff, 0xff00, 0xff0000, 0);
	SDL_SetSurfaceBlendMode(fmul, SDL_BLENDMODE_MOD);
	for(i=0; i<sztiles; i++) {
        fscanf(conf, "%s %lx %lx", fname, &fcol, &bcol);
        SDL_FillRect(fmul, NULL, fcol);
        img = SDL_LoadBMP(fname);
        SDL_BlitSurface(fmul, NULL, img, NULL);
        SDL_SetColorKey(img, SDL_TRUE, 0);
        tiles[i] = SDL_CreateRGBSurface(0, swidth, sheight, 24, 0xff, 0xff00, 0xff0000, 0);
        SDL_FillRect(tiles[i], NULL, bcol);
        SDL_BlitSurface(img, NULL, tiles[i], NULL);
        SDL_FreeSurface(img);
	}
	SDL_FreeSurface(fmul);

	chdir(oldwd);
	info("SDL initialized.");
	debug("SDL Parameters:\nTile dimensions: %dx%d\nScreen dimensions: %dx%d", twidth, theight, tcols, trows);
}
#endif

