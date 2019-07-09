#pragma once
#include <stdint.h>

typedef enum reg_type_t { 
    REG_A = 0x00,  REG_B = 0x01, REG_C = 0x02, REG_D = 0x03, 
    REG_E = 0x04,  REG_H = 0x05, REG_L = 0x06, REG_FLAGS = 0x07, 
    REG_PC = 0x08, REG_SP = 0x09, REG_M = 0x0a, REG_PSW = 0x0b
} reg_type_t;

typedef struct reg_8 {
    reg_type_t type;
    uint8_t value;

} reg_8_t;

typedef struct reg_16 {
    reg_type_t type;
    uint16_t value;

} reg_16_t;


const static char *register_strings[] = {
    "A", "B", "C", "D",
    "E", "H", "L", "FLAGS",
    "PC", "SP", "M", "PSW"

};

const char *reg_to_str(reg_type_t reg);
