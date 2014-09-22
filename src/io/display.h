//
// io/display.h
//

#ifndef IO_DISPLAY_H
#define IO_DISPLAY_H

#include "nogr/display.h"
#include "ncurses/display.h"
#include "sdl/display.h"

#define GR_MODE_NONE          0
#define GR_MODE_NCURSES       1
#define GR_MODE_MC_NCURSES    2 // monocolor
#define GR_MODE_UC_NCURSES    3 // unicode
#define GR_MODE_MC_UC_NCURSES 4 // monocolor & unicode
#define GR_MODE_SDL2D         5 // SDL 2D tile

extern int disp_width;
extern int disp_height;
extern int scroll_x;
extern int scroll_y;

void disp_init(void);
void disp_end(void);
void disp_put(int x, int y, int tile);
void disp_dim_update(void);
void disp_clear(void);
void disp_zoom(int, int);

void redraw(void);
void scroll_disp(int, int);
void scroll_center(int, int);

extern void (* graphics_end)(void);
extern void (* graphics_put)(int, int, int);
extern void (* graphics_dim_update)(int *, int *);
extern void (* graphics_clear)(void);
extern void (* graphics_zoom)(int, int);

extern void (* disp_refresh)(void);

extern void (* memo)(const char *, ...);
extern void (* reset_memos)(void);
extern void (* statline)(int, const char *, ...);

#endif
