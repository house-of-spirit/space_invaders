#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "debug/debug.h"
#include "emulator/emulator.h"
#include "instruction/instruction.h"

int emulate(arcade_t *a, uint16_t *breakpoints, bool debug)
{
    #define TO_USEC(ts) 1000000 * ts.tv_sec + ts.tv_nsec / 1000000
    
    static bool should_break = false;

    char debug_buf[2048] = {};
    char query[256] = {0};
    
    struct timespec ts;
    uint64_t prev_frame_time;
    size_t bp_length = 0;

    timespec_get(&ts, TIME_UTC);
    prev_frame_time = TO_USEC(ts);
    
    if(breakpoints != NULL)
        while(breakpoints[bp_length] != 0xffff) bp_length++;
    
    a->cycles_passed = 0;

    while(true)
    {
        if(a->cycles_passed >= 33333)
            prev_frame_time = do_frame_action(a, prev_frame_time);
            
        if(debug)
            interactive_context(a, breakpoints, &bp_length);
        
        execute_current_ins(a);    
             
    }


    return 0;
}


void execute_current_ins(arcade_t *a)
{
    ins_t instruction = {};
    uint8_t *ins_bytecode = &a->mem->mem[a->PC];
    parse_ins(&instruction, ins_bytecode);
    instruction.ins_func(a, &instruction);
    a->PC += instruction.bytecode_size; 
    a->cycles_passed += instruction.cycle_count;
}

uint64_t do_frame_action(arcade_t *a, uint64_t previous_time)
{
    struct timespec ts; 
    timespec_get(&ts, TIME_UTC);
    int64_t delta =  TO_USEC(ts) - previous_time;
    int64_t to_sleep = 16666 - delta;
    if(to_sleep >= 0) usleep(to_sleep);
            
    timespec_get(&ts, TIME_UTC);
    previous_time = TO_USEC(ts);

    a->cycles_passed = 0;

    /* render video memory *before* interrupt */
    render_frame();

    if(a->interrupt_enabled)
        trigger_interrupt(a, 2);
    
    return previous_time;
}

void render_frame() {}


void trigger_interrupt(arcade_t *a, size_t num)
{
    if(num > 7) return;
    a->SP -= 2;
    *((uint16_t*)&a->mem->mem[a->SP]) = a->PC;
    
    a->PC = num * 8; // RST num
}
