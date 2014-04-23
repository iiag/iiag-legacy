//
// faction.c
//

#include <stdlib.h>
#include <string.h>
#include "faction.h"

static faction * first = NULL;

faction * fctn_get(const char * name)
{
	faction * f;

	for (f = first; f != NULL; f = f->next) {
		if (!strcmp(name, f->name)) return f;
	}

	f = malloc(sizeof(faction));
	f->rels = NULL;
	f->name = malloc(strlen(name) + 1);
	f->next = first;
	strcpy(f->name, name);
	first = f;

	return f;
}

int fctn_relto(const faction * f, const faction * to)
{
	relation * r;
	
	if (f == NULL) return 0;
	if (f == to) return 1000;

	for (r = f->rels; r != NULL; r = r->next) {
		if (r->to == to) return r->value;
	}

	return 0;
}

void fctn_modrel(faction * f, const faction * to, int by)
{
	relation * r;

	for (r = f->rels; r != NULL; r = r->next) {
		if (r->to == to) {
			r->value += by;
			return;
		}
	}

	r = malloc(sizeof(relation));
	r->value = by;
	r->to = to;
	r->next = f->rels;
	f->rels = r;
}
