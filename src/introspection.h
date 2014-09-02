//
// introspection.h
//

#ifndef INTROSPECTION_H
#define INTROSPECTION_H

//
// Initialize introspection with argv[0] (or the program file that's loaded).
//
void init_introspection(const char *);

//
// Get the function or symbol that contains the address and write a location description
// to the string, up to the specified size. A null-terminator is always written, even if
// it truncates the string.
//
void get_location(void *, char *, int);

//
// Get a pointer to the base pointer of the numbered call frame, where 0 is the one
// immediately calling this function.
// WARNING: Calling this with a number other than 0 may cause segment violations.
// 
void *get_frame(int);

//
// Resolve the string table index to a pointer therein.
//
const char *resolve_string(int);

#define describe_current(s, sz) get_location(get_frame(0), s, sz)
#define describe_caller(s, sz) get_location(get_frame(1), s, sz)

// Don't touch this.

#endif