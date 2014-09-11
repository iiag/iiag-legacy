//
// io/display.h
//

#ifndef IO_DISPLAY_H
#define IO_DISPLAY_H

#include "nogr/display.h"
#include "ncurses/display.h"

#define GR_MODE_NONE       0
#define GR_MODE_NCURSES    1
#define GR_MODE_MC_NCURSES 2 // monocolor

extern int disp_width;
extern int disp_height;
extern int scroll_x;
extern int scroll_y;

void disp_init(void);
void disp_end(void);
void disp_put(int x, int y, int tile);
void disp_dim_update(void);
void disp_clear(void);

void redraw(void);
void scroll_disp(int, int);
void scroll_center(int, int);

extern void (* graphics_end)(void);
extern void (* graphics_put)(int, int, int);
extern void (* graphics_dim_update)(int *, int *);
extern void (* graphics_clear)(void);

extern void (* disp_refresh)(void);

extern void (* memo)(const char *, ...);
extern void (* reset_memos)(void);
extern void (* statline)(int, const char *, ...);

#endif
