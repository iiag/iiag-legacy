//
// iml.c
//

#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "iml.h"
#include "../util.h"
#include "../display.h"

// for error reporting
static const char * file;
static int line;

static void iml_error(char * fmt, ...)
{
	va_list vl;

	end_disp();

	va_start(vl, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, vl);
	fprintf(stderr, " (%s, line %d)\n", file, line);
	va_end(vl);

	exit(1);
}

static void igspaces(char ** buf)
{
redo:
	while (isspace(**buf)) {
		if (**buf == '\n') ++line;
		++*buf;
	}

	if (**buf == '#') {
		while (**buf != '\n' && **buf) ++*buf;
		line++;
		goto redo;
	}
}

static void expect(char ** buf, char c)
{
	if (**buf != c) {
		iml_error("Expected '%c', got '%c'", c, **buf);
	}
	++*buf;
}

static int is_delim(char c, char * dlms)
{
	for (; *dlms; dlms++) {
		if (c == *dlms) return 1;
	}
	return !c;
}

static char * next_token(char ** buf, char * dlms)
{
	char * tok;
	char * start;
	char * end;

	igspaces(buf);
	start = *buf;
	end = start;

	while (!is_delim(**buf, dlms)) {
		if (!isspace(**buf)) end = *buf + 1;
		++*buf;
	}

	tok = malloc(end - start + 1);
	memcpy(tok, start, end - start);
	tok[end - start] = 0;

	return tok;
}

static int set(iml_lang * l, void * thing, const char * fld, char * val)
{
	int i = iml_lang__find(l, fld);
	if (i == IML_NOT_FOUND) return 0;

	void * to = iml_lang__get_ptr(l, thing, i);

	switch (l->fields[i].type) {
	case IML_STRING:
		*(char **)to = val;
		break;

	case IML_INT:
		*(int *)to = atoi(val);
		free(val);
		break;

	case IML_CHAR:
		*(char *)to = *val;
		free(val);
		break;

	case IML_ENUM:
		*(int *)to = iml_lang__enum_val(l, i, val);

		if (*(int *)to == IML_NOT_FOUND) {
			iml_error("Unknown value for enumeration '%s'.", val);
		}

		free(val);
		break;

	default:
		// bools are handled seperatly
		assert(0);
	}

	return 1;
}

void * iml_read(const char * fn, iml_lang * lang)
{
#define REALLOC_SIZE 8

	char * org, * buf;
	char * fld, * val, * name;
	void * thing;
	void ** things;
	int i = 0;

	org = buf = read_file(fn);
	if (buf == NULL) return NULL;

	file = fn;
	line = 1;
	things = malloc(sizeof(void *));

	for (;;) {
		igspaces(&buf);
		if (!*buf) break;

		name = next_token(&buf, "{} \t\n\v\f\r");
		igspaces(&buf);
		expect(&buf, '{');

		thing = malloc(lang->struct_size);
		memcpy(thing, lang->ctor, lang->struct_size);

		if (*name) set(lang, thing, "name", name);

		// inside brackets
		do {
			fld = next_token(&buf, ",=}");

			// look for a bool
			int j = iml_lang__find(lang, fld);
			if (j == IML_NOT_FOUND && *fld == '!') {
				j = iml_lang__find(lang, fld + 1);
			}

			if (j != IML_NOT_FOUND && lang->fields[j].type == IML_BOOL) {
				// found a bool type
				expect(&buf, ',');

				unsigned * to = iml_lang__get_ptr(lang, thing, j);
				if (*fld == '!') *to &= ~lang->fields[j].bool_bit;
				else *to |= lang->fields[j].bool_bit;
			} else {
				// found a non-bool
				expect(&buf, '=');

				val = next_token(&buf, ",}");

				if (!set(lang, thing, fld, val)) {
					iml_error("Unknown field '%s'.", fld);
				}

				free(fld);

				if (*buf) buf++;
				else break;
			}
		} while (buf[-1] != '}');

		// add thing to list of things
		if (i % REALLOC_SIZE == 0) {
			things = realloc(things, (i + REALLOC_SIZE + 1) * sizeof(void *));
		}
		things[i++] = thing;
	}

	free(org);
	things[i] = NULL;

	return things;
}
