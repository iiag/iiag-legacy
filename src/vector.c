//
// vector.c
//

#include <stdlib.h>
#include "util.h"

#define REALLOC_SIZE 16

void vector_init(vector_t * v) {
	v->arr = NULL;
	v->cnt = 0;
}

vector_t * vector_new(void) {
	vector_t * v = malloc(sizeof(vector_t));
	vector_init(v);
	return v;
}

void vector_free(vector_t * v) {
	free(v->arr);
	free(v);
}

void vector_append(vector_t * v, void * x) {
	if (v->cnt % REALLOC_SIZE == 0) {
		v->arr = realloc(v->arr, (v->cnt + REALLOC_SIZE) * sizeof(void *));
	}
	v->arr[v->cnt++] = x;
}
