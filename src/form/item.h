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
	EQUIPABLE,
} item_type;

typedef struct iform {
	is_form;
	item_type type;

	union {
		// consumable-specific data
		struct {
			int restore_health;
			int restore_stamina;
		} cn;

		// equipable-specific data
		struct {
			int modify_attack;
			int modify_ac;
			int slot;
		} eq;
	} u;
} iform;

iform * iform_new(item_type, chtype);
void iform_free(iform *);

#endif
