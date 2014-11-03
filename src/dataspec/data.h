//
// dataspec/data.h
//

#ifndef DATASPEC_DATA_H
#define DATASPEC_DATA_H

typedef struct data_spec data_spec_t;
typedef struct data_class data_class_t;
typedef struct data_field data_field_t;

//
// The functions of this type are intended to act as follows:
//   constructor(from) :=
//     NULL : default
//     from : copy(from)
//
typedef void * (constructor_t *)(void *);

//
// The destructor to reflect the constructor
typedef void (destructor_t *)(void *);

#include <stddef.h>
#include "../util.h"

struct data_field {
	char * name;
	size_t offset;
};

struct data_class {
	char * name;
	constructor_t construct;
	destructor_t  destruct;
	vector_t * array;
	vector_t fields;
};

struct data_spec {
	vector_t classes;
};

// Creates an empty data specification format
data_spec_t * make_data_spec(void);

// Adds a class to a data specification format
data_class_t * add_data_class(data_spec_t *, const char *, constructor_t, destructor_t, vector_t *);

// Adds a field to a data class
void add_data_field(data_class_t *, const char *, size_t);

// Loads the data from the file
void load_data(const char *, data_spec_t *);

// A nice wrapper for add_data_field
#define add_data_sfield(CLS, NM, ST, FLD) \
	add_data_field(CLS, NM, offsetof(ST, FLD))

#endif
