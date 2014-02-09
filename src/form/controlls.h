//
// form/controlls.h
//

#ifndef FORM_CONTROLLS_H
#define FORM_CONTROLLS_H

typedef struct {
	const char* filename;
	const char* playername;
}fileInfo;

typedef struct {
	char* nameType;
	char ch;
}charInfo;

void readFile(const char* filename);
void parseSource(const char* source);
void createFile();

#define charInfo* control;

#endif
