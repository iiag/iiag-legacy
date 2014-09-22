//
// io/nogr/input.h
//

#ifndef IO_NOGR_INPUT_H
#define IO_NOGR_INPUT_H

#include "../../creature.h"
#include "../../inventory.h"

const char * sdl_name_from_key(int key);
int sdl_key_from_name(const char * str);
int sdl_get_key(void);
int sdl_get_ctrl(void);

int sdl_prompt_dir(const char * prompt, int * px, int * py);
int sdl_prompt_inv(const char * prompt, inventory * inv, creature * c);
void sdl_prompt_equipped(const char * prompt, creature * c);
char * sdl_prompt_string(const char * prompt);

#endif
