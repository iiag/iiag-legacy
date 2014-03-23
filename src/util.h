//
// util.h
//

struct vector;

#ifndef UTIL_H
#define UTIL_H

typedef struct vector {
	void ** arr;
	int cnt;
} vector_t;

void vector_init(vector_t *);
vector_t * vector_new(void);
void vector_free(vector_t *);
void vector_append(vector_t *, void *);

void * choose_random(vector_t *, int, int);

//
// Given a filename, will read that file into a buffer and return that buffer
// It is left to the caller to free the returned buffer
//
char * read_file(const char *);

//
// Allocates memory for a string and copies the given string to that string
// Returns the new string, which should be freed
//
char * copy_str(const char *);

#endif
