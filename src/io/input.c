//
// input.c
//

#include "input.h"

static void dumb() {};

const char * (* name_from_key)(int) = (void *)dumb;
int (* key_from_name)(const char *) = (void *)dumb;

int (* input_get_key)(void) = (void *)dumb;
int (* input_get_ctrl)(void) = (void *)dumb;
int (* input_prompt_dir)(const char *, int *, int *) = (void *)dumb;
int (* input_prompt_inv)(const char *, inventory *, creature *) = (void *)dumb;
void (* input_prompt_equipped)(const char *, creature * c) = dumb;
char * (* input_prompt_string)(const char *) = (void *)dumb;

void (* input_display_controls)(void) = dumb;
void (* input_prompt_save_controls)(void) = dumb;
