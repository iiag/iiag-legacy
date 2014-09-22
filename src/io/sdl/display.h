//
// io/sdl/display.h
//

#ifndef IO_SDL_DISPLAY_H
#define IO_SDL_DISPLAY_H

void sdl_init(FILE *);

#ifdef WITH_SDL

#include <SDL2/SDL_ttf.h>

extern unsigned int swidth, sheight, twidth, theight, trows, tcols;
extern TTF_Font *font;
extern SDL_Surface *disp;
extern SDL_Window *win;

void sdl_memo(const char * fmt, ...);

#endif

#endif
