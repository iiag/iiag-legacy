//
// io/ncurses/input.h
//

#ifndef IO_NCURSES_INPUT_H
#define IO_NCURSES_INPUT_H

#ifdef WITH_NCURSES

#include <ncurses.h>
#include "../../creature.h"
#include "../../inventory.h"

const char * nc_name_from_key(int);
int nc_key_from_name(const char *);

int nc_get_key(void);
int nc_get_ctrl(void);
int nc_prompt_dir(const char * prompt, int * dx, int * dy);
int nc_prompt_inv(const char * prompt, inventory * inv, creature * c);
void nc_prompt_equipped(const char * msg, creature * c);
char * nc_prompt_string(const char *);

#endif // #ifdef USE_NCURSES
#endif // #ifndef IO_NCURSES_INPUT_H
