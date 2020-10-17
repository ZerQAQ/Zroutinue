#include "type.h"

#define list_init(o)\
    o->next = o->prev = o;

#define list_del()