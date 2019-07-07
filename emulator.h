#pragma once
#include "arcade.h"
#include "instruction.h"


int emulate(arcade_t *arcade, uint16_t *breakpoints, bool context);
void execute_ins(arcade_t *arcade, struct ins *instruction);

