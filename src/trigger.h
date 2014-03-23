//
// trigger.h
//

#ifndef TRIGGER_H
#define TRIGGER_H

typedef int trigger;

//
// Fires the given trigger
//
void trigger_pull(trigger, void *);

#endif
