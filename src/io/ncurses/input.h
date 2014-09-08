//
// io/ncurses/input.h
//

#ifndef IO_NCURSES_INPUT_H
#define IO_NCURSES_INPUT_H

#ifdef USE_NCURSES

#include <ncurses.h>
#include "../../creature.h"
#include "../../inventory.h"

int nc_get_ctrl(void);
int nc_prompt_dir(const char * prompt, int * dx, int * dy);
int nc_prompt_inv(const char * prompt, inventory * inv, creature * c);
void nc_prompt_equipped(const char * msg, creature * c);
char * nc_prompt_command(void);

#define KB_q 'q'
#define KB_w 'w'
#define KB_e 'e'
#define KB_r 'r'
#define KB_t 't'
#define KB_y 'y'
#define KB_u 'u'
#define KB_i 'i'
#define KB_o 'o'
#define KB_p 'p'

#define KB_a 'a'
#define KB_s 's'
#define KB_d 'd'
#define KB_f 'f'
#define KB_g 'g'
#define KB_h 'h'
#define KB_j 'j'
#define KB_k 'k'
#define KB_l 'l'

#define KB_z 'z'
#define KB_x 'x'
#define KB_c 'c'
#define KB_v 'v'
#define KB_b 'b'
#define KB_n 'n'
#define KB_m 'm'

#define KB_Q 'Q'
#define KB_W 'W'
#define KB_E 'E'
#define KB_R 'R'
#define KB_T 'T'
#define KB_Y 'Y'
#define KB_U 'U'
#define KB_I 'I'
#define KB_O 'O'
#define KB_P 'P'

#define KB_A 'A'
#define KB_S 'S'
#define KB_D 'D'
#define KB_F 'F'
#define KB_G 'G'
#define KB_H 'H'
#define KB_J 'J'
#define KB_K 'K'
#define KB_L 'L'

#define KB_Z 'Z'
#define KB_X 'X'
#define KB_C 'C'
#define KB_V 'V'
#define KB_B 'B'
#define KB_N 'N'
#define KB_M 'M'

#define KB_SPACE  ' '
#define KB_COMMA  ','
#define KB_PERIOD '.'
#define KB_COLON  ':'

#define KB_LEFT  KEY_LEFT
#define KB_RIGHT KEY_RIGHT
#define KB_UP    KEY_UP
#define KB_DOWN  KEY_DOWN

#endif // #ifdef USE_NCURSES
#endif // #ifndef IO_NCURSES_INPUT_H
