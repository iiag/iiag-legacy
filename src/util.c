//
// util.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

void * choose_random(vector_t * v, int offset, int max)
{
	int i, x;
	int r = random() % max;

	for (i = 0; i < v->cnt; i++) {
		x = *(int *)((char *)(v->arr[i]) + offset);
		if (r < x) break;
		r -= x;
	}

	assert(i != v->cnt);
	return v->arr[i];
}

char * read_file(const char * fn)//reading the file contaning teh list of creatures, monsters, and generic items 
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

char * copy_str(const char * old)//copy string
{
	char * str = malloc(strlen(old) + 1);
	strcpy(str, old);
	return str;
}
