//
// form/item.h
//

struct iform;

#ifndef FORM_ITEM_H
#define FORM_ITEM_H

#include "form.h"

typedef enum {
	USELESS,
} item_type;

typedef struct iform {
	is_form;
	item_type type;
} iform;

iform * iform_new(item_type, chtype);
void iform_free(iform *);

#endif
