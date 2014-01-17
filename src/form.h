//
// form.h
//

#ifndef FORM_H
#define FORM_H

#include <ncurses.h>

typedef enum {
	NONE = -1,
	USELESS,
	CREATURE,
} obj_type;

typedef struct {
	int refs;
	int weight;
	obj_type type;
	chtype ch;
} form;

form * form_new(obj_type, chtype);
form * form_copy(form *);
void form_free(form *);

#endif
