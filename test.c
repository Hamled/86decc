#include "86decc.h"
#include "utest.h"

UTEST_MAIN();

UTEST(foo, bar) {
    const uint8_t input[] = { 0x01, 0xD8 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ(output.opcode, (enum OPCODE_t)OP_ADD);
}
