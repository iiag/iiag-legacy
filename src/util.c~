//
// util.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

char * read_file(const char * fn)
{
	FILE * f;
	char * buf;
	size_t size;

	f = fopen(fn, "r");
	if (f == NULL) return NULL;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = malloc(size + 1);
	fread(buf, 1, size, f);
	buf[size] = 0;

	fclose(f);

	return buf;
}

char * copy_str(const char * old)
{
	char * str = malloc(strlen(old) + 1);
	strcpy(str, old);
	return str;
}
