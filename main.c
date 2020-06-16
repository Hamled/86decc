#include "86decc.h"
#include "debug.h"

#include <stdio.h>

uint8_t TEST1[] = { 0x61, 0x61, 0x98, 0x60, 0x99, 0x61, 0x60, 0x98, 0x60 };

int main() {
    INSTR instr = {0};

    uint8_t* ixns = TEST1;
    const size_t n = sizeof(TEST1);
    while(ixns < (TEST1 + n)) {
        size_t ixn_size = decode(ixns, &instr);
        if(ixn_size < 1) {
            printf("Could not decode instruction starting with: %hhx\n", *ixns);
            return EXIT_FAILURE;
        }
        print_instr(&instr);

        ixns += ixn_size;
    }

    return EXIT_SUCCESS;
}
