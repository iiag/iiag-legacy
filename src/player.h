//
// player.h
//

#ifndef PLAYER_H
#define PLAYER_H

#include "world.h"
#include "creature.h"

#define PLYR world.plyr
#define plyr_is_me(C) ((C) == &PLYR)

// player willed actions
void plyr_act_pickup(void);
void plyr_act_drop(void);
void plyr_act_inv(void);
void plyr_act_move(int, int);
void plyr_act_consume(void);

// the inevitable
void plyr_ev_death(const char *);
void plyr_ev_lvlup(void);

#endif
