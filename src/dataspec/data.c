//
// dataspec/data.c
//

#include <stdlib.h>
#include "data.h"
#include "../log.h"

data_spec_t * make_data_spec(void)
{
	data_spec_t * sp = malloc(sizeof(data_spec_t));
	vector_init(&sp);
	return sp;
}

data_class_t * add_data_class(data_spec_t * sp, const char * name, constructor_t construct, destructor_t destruct, vector_t * arr)
{
	data_class_t * cl = malloc(sizeof(data_class_t));

	cl->construct = construct;
	cl->destruct = destruct;
	cl->array = arr;
	cl->name = malloc(strlen(name) + 1);
	strcpy(cl->name, name);
	vector_init(&cl->fields);

	vector_append(&sp->classes, cl);
	return cl;
}

void add_data_field(data_class_t * cl, data_type_t type, const char * name, size_t offset)
{
	data_field_t * fld = malloc(sizeof(data_field_t));

	fld->type = type;
	fld->offset = offset;
	fld->name = malloc(strlen(name) + 1);
	strcpy(fld->name, name);

	vector_append(&cl->fields, fld);
}

static int nextc(FILE * f)
{
	int c;
	do {
		c = fgetc(f);
	} while (isspace(c));
	return c;
}

static int peekc(FILE * f)
{
	int c = nextc(f);
	ungetc(c, f);
	return c;
}

static char * read_delim_str(FILE * f, const char * delims)
{
#define GROW_SIZE 64
	int i = 0;
	char * buf = NULL;

	peekc(f);

	for (;;) {
		if (i % GROW_SIZE == 0) {
			buf = realloc(buf, i + GROW_SIZE);
		}

		buf[i] = fgetc(f);
		if (strchr(delims, buf[i]) != NULL) break;
		if (buf[i] == EOF) break;
		i++;
	}

	ungetc(buf[i], f);
	buf[i] = 0;
	return buf;
#undef GROW_SIZE
}

static void chomp(char * str)
{
	int i = strlen(str) - 1;
	while (i >= 0 && isspace(str[i])) {
		str[i] = 0;
		i--;
	}
}

static void load_classes(FILE * f, int delim, void * context, data_class_t * class, data_spec_t * ds)
{
	data_field_t * fld;
	data_class_t * cl;
	void * sub;
	char * ident;
	char * inst_name;

	while (peekc(f) != EOF && peekc(f) != delim) {
		ident = read_delim_str(f, " \t\n\v\f\r=");

		if (fgetc(f) == '=') {
			// Trying to specify the value of a field
			if (!context) fatal("Can only specify field values within a structure.");

			// Search for the field
			for (i = 0; i < class->fields.cnt; i++) {
				fld = class->fields.arr[i];
				if (!strcmp(fld->name, ident)) break;
			}

			if (i == class->fields.cnt) {
				fatal("Field name '%s' unrecognized.", ident);
			}

			// Set the field
			value = read_delim_str(f, " \t\n\v\f\r");

			switch (fld->type) {
			case DATA_INT:
				*(int *)((char *)context + fld->offset) = atoi(value);
				free(value);
				break;
			case DATA_STRING:
				*(char **)((char *)context + fld->offset) = value;
				break;
			default:
				fatal("Unsupported field type %d.", fld->type);
			}
		} else {
			// Trying to specify a class
			item_name = read_delim_str(f, "{/");
			chomp(item_name);

			// Search for which class we have
			for (i = 0; i < ds->classes.cnt; i++) {
				cl = ds->classes.arr[i];
				if (!strcmp(cl->name, ident)) break;
			}

			if (i == ds->classes.cnt) {
				fatal("Class name '%s' unrecognized.", ident);
			}

			if (class != NULL && cl != class) {
				fatal("Class mismatch of '%s' and '%s' in subinstance.", class->name, ident);
			}

			sub = cl->construct(context);
			if (fgetc(f) == '{') {
				load_classes(f, '}', sub, cl ds);
				cl->destruct(context);
			} else {
				load_classes(f, '/', sub, cl, ds);
				vector_append(cl->array, sub);
			}

			free(item_name);
		}

		free(ident);
	}
}

void load_data(const char * filename, dataspec_t * ds)
{
	FILE * f;

	f = fopen(filename, "rb");
	if (f == NULL) {
		fatal("Cannot open data file '%s'.\n", filename);
	}

	load_classes(f, EOF, NULL, ds);

	fclose(f);
}

