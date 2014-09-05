//
// input.h
//

#ifndef INPUT_H
#define INPUT_H

#include "../creature.h"

//
// Gets a keypress and translates it into a control value
//
int get_ctrl(void);

//
// Prompts for valid directional input
//
int prompt_dir(const char *, int *, int *);

//
// Will display the inventory an prompt, and return the item selected by the user
//
int prompt_inv(const char *, inventory *, struct creature *);

char * prompt_command(void);

#endif
