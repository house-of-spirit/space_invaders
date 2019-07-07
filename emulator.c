#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "debug.h"
#include "emulator.h"
#include "instruction.h"

int emulate(arcade_t *a, uint16_t *breakpoints, bool context)
{
    #define TO_USEC(ts) 1000000 * ts.tv_sec + ts.tv_nsec / 1000000
    
    static bool should_break = false;

    char context_buf[2048] = {};
    char query[256] = {0};
    
    struct timespec ts;
    uint64_t ms1;

    int cnt = 0;
    size_t bp_length = 0;

    timespec_get(&ts, TIME_UTC);
    ms1 = TO_USEC(ts);


    while(breakpoints[bp_length] != 0xffff) bp_length++;
    
    a->cycles_passed = 33333;

    while(true)
    {
         
        if(a->cycles_passed >= 33333)
        {
            // screen interrupt
            timespec_get(&ts, TIME_UTC);

            int64_t delta =  ms1 - TO_USEC(ts);

            int64_t to_sleep = 16666 - delta;
            
            if(to_sleep >= 0) usleep(to_sleep);
            
            timespec_get(&ts, TIME_UTC);
            ms1 = TO_USEC(ts);

            a->cycles_passed = 0;
            cnt++;
            if(cnt%60==0) printf("%d\n", cnt);
        }
        
        
        if(context)
        {
            interactive_context(a, breakpoints, &bp_length);
        }
        

        ins_t instruction = {};
        uint8_t *ins_bytecode = &a->mem->mem[a->PC];
        parse_ins(&instruction, ins_bytecode);
        instruction.ins_func(a, &instruction);
        a->PC += instruction.bytecode_size; 
        a->cycles_passed += instruction.cycle_count;
         
    }


    return 0;
}


