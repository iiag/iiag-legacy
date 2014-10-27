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

data_class_t * add_data_class(data_spec_t * sp, const char * name, constructor_t cons, vector_t * arr)
{
	data_class_t * cl = malloc(sizeof(data_class_t));

	cl->name = malloc(strlen(name) + 1);
	strcpy(cl->name, name);
	cl->construct = cons;
	cl->array = arr;
	vector_init(&cl->fields);

	vector_append(&sp->classes, cl);
	return cl;
}

static char * read_delim_str(FILE * f, const char * delims)
{
#define GROW_SIZE 64
	int i = 0;
	char * buf = NULL;

	for (;;) {
		if (i % GROW_SIZE == 0) {
			buf = realloc(buf, i + GROW_SIZE);
		}

		buf[i] = fgetc(f);
		if (strchr(delims, buf[i]) != NULL) break;
		i++;
	}

	ungetc(buf[i], f);
	buf[i] = 0;
	return buf;
#undef GROW_SIZE
}

static void ignore_spaces(FILE * f)
{
	int c;
	do {
		c = fgetc(f);
	} while (isspace(f));
	ungetc(c, f);
}

static void not_eof(FILE * f, const char * filename)
{
	if (feof(f)) {
		fatal("Unexpected end-of-file in \"%s\"", filename);
	}
}

void load_data(const char * filename, dataspec_t * ds)
{
	char * class_name;
	char * item_name;
	FILE * f;

	f = fopen(filename, "rb");
	if (f == NULL) fatal("Cannot open data file '%s'.\n", filename);

	for (;;) {
		class_name = read_delim_str(f, " \t\n\v\f\r");
		not_eof(f, filename);
		ignore_spaces(f);
		item_name = read_delim_str(f, "./");
	}

	fclose(f);
}

