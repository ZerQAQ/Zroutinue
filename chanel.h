#include "type.h"

typedef struct{
    u64 data_size, len, cap, ptr;
    u8 *data;
} __Chanel;

#define 