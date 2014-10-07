//
// trigger.c
///

#include <assert.h>
#include "log.h"
#include "trigger.h"
#include "sol/cdata.h"
#include "solint.h"

//
// Tries to call sol function then C function
//
void trigger_pull(const trigger * tr, creature * ud1, void * ud2)
{
    /*
    Hello Graham. This is the Land Fiish

    How has the Sol been going?
    I will be here 24/8

    sooo...Bai!

    Swim Deep
    ~Land Fiish

    */

    int error;
	// call sol function
    if(tr->sol_func) {
        sol_object_t *res = sol_util_call(sol_state, tr->sol_func, &error, 1, sol_new_cstruct(sol_state, ud1, sol_crtr_specs));
        if(error) {
            warning("Sol trigger error: %s", (sol_is_string(res)?res->str:"<printed to stdout>"));
            if(!sol_is_string(res)) ob_print(res);
        }
        sol_obj_free(res);
    }

	// call c function
	if (tr->c_func != NULL) {
		tr->c_func(ud1, ud2);
	}
}
