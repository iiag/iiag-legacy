//
// input.h
//

#ifndef INPUT_H
#define INPUT_H

#include "ncurses/input.h"
#include "../creature.h"
#include "../inventory.h"

// returns the nice name of a key
// The names are not garunteed to be preserved across multiple calls
extern const char * (* name_from_key)(int);

// returns the key from a key name
extern int (* key_from_name)(const char *);

// gets an always blocked keypress from the user
extern int (* input_get_key)(void);

// gets a control from the user
// it is blocking normally, nonblocking in real time mode
extern int (* input_get_ctrl)(void);

// prompts the player for input from a direction
extern int (* input_prompt_dir)(const char *, int *, int *);

// prompts the player for input from the inventory
extern int (* input_prompt_inv)(const char *, inventory *, creature *);

// displays the equipment of the creature
extern void (* input_prompt_equipped)(const char *, creature * c);

// prompts for a string
extern char * (* input_prompt_string)(const char *);

// for modifying controls ingame
extern void (* input_display_controls)(void);
extern void (* input_prompt_save_controls)(void);

#endif
