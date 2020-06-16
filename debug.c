#include "debug.h"
#include "86decc.h"

#include <stdio.h>

void print_instr(INSTR* instr) {
    if(!instr) abort();

    switch(instr->opcode) {
        case OP_UNKNOWN:
            puts("--UNKNOWN--BUG--");
            break;
        case OP_PUSHA:
            puts("PUSHA");
            break;
        case OP_POPA:
            puts("POPA");
            break;
        case OP_PUSHAD:
            puts("PUSHAD");
            break;
        case OP_POPAD:
            puts("POPAD");
            break;
        case OP_CBW:
            puts("CBW");
            break;
        case OP_CWDE:
            puts("CWDE");
            break;
        case OP_CWD:
            puts("CWD");
            break;
        case OP_CDQ:
            puts("CDQ");
            break;
        case OP_ADD:
            puts("ADD");
            break;
        case OP_OR:
            puts("OR");
            break;
        case OP_ADC:
            puts("ADC");
            break;
        case OP_AND:
            puts("AND");
            break;
        case OP_CMP:
            puts("CMP");
            break;
        case OP_SBB:
            puts("SBB");
            break;
        case OP_SUB:
            puts("SUB");
            break;
        case OP_XOR:
            puts("XOR");
            break;
    }
}
