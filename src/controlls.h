//
//controlls.h
//

#ifndef CONTROLLS_H
#define CONTROLLS_H

int duplicates();
void display_controls();
void draw_controls(int offset,int spot,int ysize);
void check_spot(int *spot,int *offset,int ysize);
void check_offset(int *offset,int ysize);
void prompt_save_controls();
void save_controls(char* name);

struct str_pairs{
	const char* user_CTRL;
	const char*	enum_CTRL;
	int ctrl_CTRL;
};

extern struct str_pairs controls[];

#endif