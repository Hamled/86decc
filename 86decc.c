#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum OPCODE_t {
    PUSHA,
    PUSHAD,
    POPA,
    POPAD,
    CBW,
    CWDE,
    CWD,
    CDQ,
} OPCODE;

typedef struct INSTR_t {
    OPCODE opcode;
} INSTR;


// Decode a single instruction from the byte stream ixns
// into the INSTR structure passed by reference
// This assumes that instr has been initialized to all zeroes
size_t decode(const uint8_t* ixns, INSTR* instr) {
    if(!instr) abort();

    const uint8_t ixn = *ixns;

    const uint8_t opcode = ixn >> 3;
    switch(opcode) {
        case 0b01100:
            // PUSHAD or POPAD
            instr->opcode =
                ixn & 0x7 ? POPAD : PUSHAD;
            return 1;
        case 0b10011:
            // CWDE or CDQ
            instr->opcode =
                ixn & 0x7 ? CDQ : CWDE;
            return 1;
    }

    abort();
}

void print_instr(INSTR* instr) {
    if(!instr) abort();

    switch(instr->opcode) {
        case PUSHA:
            puts("PUSHA");
            break;
        case POPA:
            puts("POPA");
            break;
        case PUSHAD:
            puts("PUSHAD");
            break;
        case POPAD:
            puts("POPAD");
            break;
        case CBW:
            puts("CBW");
            break;
        case CWDE:
            puts("CWDE");
            break;
        case CWD:
            puts("CWD");
            break;
        case CDQ:
            puts("CDQ");
            break;
    }
}

uint8_t TEST1[] = { 0x61, 0x61, 0x98, 0x60, 0x99, 0x61, 0x60, 0x98, 0x60 };

int main() {
    INSTR instr = {0};

    uint8_t* ixns = TEST1;
    const size_t n = sizeof(TEST1);
    while(ixns < (TEST1 + n)) {
        size_t ixn_size = decode(ixns, &instr);
        print_instr(&instr);

        ixns += ixn_size;
    }

    return 0;
}
