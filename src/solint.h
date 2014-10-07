#ifndef SOLINT_H
#define SOLINT_H

#include "sol/sol.h"

extern sol_state_t *sol_state;

extern sol_object_t *sol_crtr_specs;
extern sol_object_t *sol_item_specs;

void init_sol(void);

sol_object_t *solcf_creature(sol_state_t *, sol_object_t *);
sol_object_t *solcf_player(sol_state_t *, sol_object_t *);
sol_object_t *solcf_item(sol_state_t *, sol_object_t *);
sol_object_t *solcf_room(sol_state_t *, sol_object_t *);
sol_object_t *solcf_i_type(sol_state_t *, sol_object_t *);
sol_object_t *solcf_begin_recipe(sol_state_t *, sol_object_t *);
sol_object_t *solcf_component(sol_state_t *, sol_object_t *);

sol_object_t *solcf_debug(sol_state_t *, sol_object_t *);
sol_object_t *solcf_info(sol_state_t *, sol_object_t *);
sol_object_t *solcf_notice(sol_state_t *, sol_object_t *);
sol_object_t *solcf_warning(sol_state_t *, sol_object_t *);
sol_object_t *solcf_error(sol_state_t *, sol_object_t *);
sol_object_t *solcf_fatal(sol_state_t *, sol_object_t *);
sol_object_t *solcf_memo(sol_state_t *, sol_object_t *);

#endif
