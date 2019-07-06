#include <stddef.h>

#include "register.h"

const char *reg_to_str(reg_type_t reg)
{
    return register_strings[(size_t)reg];
}
