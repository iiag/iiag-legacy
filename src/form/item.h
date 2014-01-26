//
// form/item.h
//

struct iform;

#ifndef FORM_ITEM_H
#define FORM_ITEM_H

#include "form.h"

#define ITEM_CONSUMABLE 1
#define ITEM_EQUIPABLE  2

typedef struct iform {
	is_form;
	unsigned type;

	// consumable-specific data
	int restore_health;
	int restore_stamina;

	// equipable-specific data
	int modify_attack;
	int modify_ac;
	int slot;
} iform;

iform * iform_new(unsigned, chtype);
void iform_free(iform *);

#endif
