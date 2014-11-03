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

static void ignore_spaces(FILE * f)
{
	int c;
	do {
		c = fgetc(f);
	} while (isspace(f));
	ungetc(c, f);
}

static char * read_delim_str(FILE * f, const char * delims)
{
#define GROW_SIZE 64
	int i = 0;
	char * buf = NULL;

	ignore_spaces(f);

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

static void chomp(char * str)
{
	int i = strlen(str) - 1;
	while (i >= 0 && isspace(str[i])) {
		str[i] = 0;
		i--;
	}
}

static void load_classes(FILE * f, int delim, void * context, dataspec_t * ds)
{
	data_class_t * cl;
	void * sub;
	char * ident;
	char * inst_name;

	while (peekc(f) != EOF && peekc(f) != delim) {
		ident = read_delim_str(f, " \t\n\v\f\r=");

		if (peekc(f) == '=') {
			// Trying to specify the value of a field
			if (!context) fatal("Can only specify field values within a structure.");


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

			sub = cl->construct(context);
			if (fgetc(f) == '{') {
				load_classes(f, '}', sub, ds);
				cl->destruct(context);
			} else {
				load_classes(f, '/', sub, ds);
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

