//
// form/item.h
//

struct iform;

#ifndef FORM_ITEM_H
#define FORM_ITEM_H

#include "form.h"

typedef enum {
	USELESS,
	CONSUMABLE,
} item_type;

typedef struct iform {
	is_form;
	item_type type;
	int restore_health;
	int restore_stamina;
} iform;

iform * iform_new(item_type, chtype);
void iform_free(iform *);

#endif
