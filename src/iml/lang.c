//
// iml/lang.c
//

#include <stdlib.h>
#include <string.h>
#include "lang.h"

iml_lang * iml_lang__new(int struct_size, void * ctor)
{
	iml_lang * lang = malloc(sizeof(iml_lang));

	lang->struct_size = struct_size;
	lang->ctor = ctor;
	lang->field_cnt = 0;
	lang->fields = NULL;

	return lang;
}

void iml_lang_free(iml_lang * lang)
{
	free(lang->fields);
	free(lang);
}

void iml_lang__add(iml_lang * l, iml_type type, char * name, int off)
{
#define REALLOC_SIZE 8

	if (l->field_cnt % REALLOC_SIZE == 0) {
		l->fields = realloc(l->fields, (l->field_cnt + REALLOC_SIZE) * sizeof(iml_field));
	}

	int i = l->field_cnt++;
	l->fields[i].type = type;
	l->fields[i].name = name;
	l->fields[i].offset = off;
#undef REALLOC_SIZE
}

void iml_lang__add_enum(iml_lang * l, char * name, int off, char ** ev, int ec)
{
	iml_lang__add(l, IML_ENUM, name, off);

	int i = l->field_cnt - 1;
	l->fields[i].enum_strv = ev;
	l->fields[i].enum_strc = ec;
}

int iml_lang__find(iml_lang * l, const char * name)
{
	int i;

	for (i = 0; i < l->field_cnt; i++) {
		if (!strcmp(l->fields[i].name, name)) {
			return i;
		}
	}

	return IML_NOT_FOUND;
}

void * iml_lang__get_ptr(iml_lang * l, void * base, int fld)
{
	return (void *)((char *)base + l->fields[fld].offset);
}

int iml_lang__enum_val(iml_lang * l, int fld, const char * enm)
{
	int i;

	for (i = 0; i < l->fields[fld].enum_strc; i++) {
		if (!strcmp(l->fields[fld].enum_strv[i], enm)) return i;
	}

	return IML_NOT_FOUND;
}
