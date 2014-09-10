//
// io/nogr/input.h
//

#ifndef IO_NOGR_INPUT_H
#define IO_NOGR_INPUT_H

#include "../../creature.h"
#include "../../inventory.h"

const char * nogr_name_from_key(int key);
int nogr_key_from_name(const char * str);
int nogr_get_key(void);
int nogr_get_ctrl(void);

int nogr_prompt_dir(const char * prompt, int * px, int * py);
int nogr_prompt_inv(const char * prompt, inventory * inv, creature * c);
void nogr_prompt_equipped(const char * prompt, creature * c);
char * nogr_prompt_string(const char * prompt);

#endif
