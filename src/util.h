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

char * read_file(const char *);
char * copy_str(const char *);

#endif
