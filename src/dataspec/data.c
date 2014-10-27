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

static int peekc(FILE * f)
{
	int c = fgetc(f);
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

static void load_classes(FILE * f, void * context)
{
	data_class_t * cl;
	char * class_name;
	char * item_name;

	while (peekc(f) != EOF) {
		class_name = read_delim_str(f, " \t\n\v\f\r");
		item_name = read_delim_str(f, "./=");
		chomp(item_name);

		for (i = 0; i < ds->classes.cnt; i++) {
			cl = ds->classes.arr[i];
			if (!strcmp(cl->name, class_name)) break;
		}

		if (i == ds->classes.cnt) {
			fatal("Class name '%s' unrecognized.", class_name);
		}

		if (fgetc(f) == '/') {
			
		} else {
		}

		free(class_name);
		free(item_name);
	}
}

void load_data(const char * filename, dataspec_t * ds)
{
	FILE * f;

	f = fopen(filename, "rb");
	if (f == NULL) fatal("Cannot open data file '%s'.\n", filename);

	load_classes(f, NULL);

	fclose(f);
}

