#pragma once
#include <arcade.h>
#include <instruction/instruction.h>


int emulate(arcade_t *arcade, uint16_t *breakpoints, bool debug);
void execute_current_ins(arcade_t *a);
uint64_t do_frame_action(arcade_t *a, uint64_t previous_time);
void trigger_interrupt(arcade_t *a, size_t num);
void render_frame();
