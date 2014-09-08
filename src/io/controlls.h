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

#endif
