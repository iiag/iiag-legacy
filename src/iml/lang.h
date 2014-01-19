//
// iml/lang.h
//

#ifndef IML_LANG_H
#define IML_LANG_H

#include <stddef.h>

typedef enum {
	IML_STRING,
	IML_INT,
	IML_CHAR,
	IML_ENUM,
} iml_type;

typedef struct {
	iml_type type;
	char * name;
	int offset;
	char ** enum_strv;
	int enum_strc;
} iml_field;

typedef struct {
	void * ctor;
	int struct_size;
	int field_cnt;
	iml_field * fields;
} iml_lang;

//
// STRCT is the name of a struct
// CTOR * to struct w/ default values to assign
//
#define iml_lang_new(STRCT, CTOR) \
	iml_lang__new(sizeof(STRCT), CTOR)

//
// LANG the iml_lang *
// TY is a iml_type
// STR is a char *, the name of the field
// STRCT should be the same as w/ iml_lang_new
// FLD is a field name in the struct
//
#define iml_lang_add(LANG, TY, STR, STRCT, FLD) \
	iml_lang__add(LANG, TY, STR, offsetof(STRCT, FLD))

//
// LANG the iml_lang *
// STR is a char *, the name of the field
// STRCT should be the same as w/ iml_lang_new
// FLD is a field name in the struct
// ENUMS is an array of strings for the enumeration
//
#define iml_lang_add_enum(LANG, STR, STRCT, FLD, ENUMS) \
	iml_lang__add_enum(LANG, STR, \
		offsetof(STRCT, FLD), \
		ENUMS, sizeof(ENUMS)/sizeof(*ENUMS) \
	)

void iml_lang_free(iml_lang *);

// very secret
iml_lang * iml_lang__new(int, void *);
void iml_lang__add(iml_lang *, iml_type, char *, int);
void iml_lang__add_enum(iml_lang *, char *, int, char **, int);

// for use in other iml functions
#define IML_NOT_FOUND (-1)

int iml_lang__find(iml_lang *, const char *);
void * iml_lang__get_ptr(iml_lang *, void *, int);
int iml_lang__enum_val(iml_lang *, int, const char *);

#endif
