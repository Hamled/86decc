#include "86decc.h"

#include <stdlib.h>
#include <stdio.h>

REGISTER regbits_to_enum_w(uint8_t bits, OPSIZE size, bool w) {
    switch(bits) {
        case 0b000:
            if(!w) return REG_AL;
            return (size == SIZE_16) ? REG_AX : REG_EAX;
        case 0b001:
            if(!w) return REG_CL;
            return (size == SIZE_16) ? REG_CX : REG_ECX;
        case 0b010:
            if(!w) return REG_DL;
            return (size == SIZE_16) ? REG_DX : REG_EDX;
        case 0b011:
            if(!w) return REG_BL;
            return (size == SIZE_16) ? REG_BX : REG_EBX;
        case 0b100:
            if(!w) return REG_AH;
            return (size == SIZE_16) ? REG_SP : REG_ESP;
        case 0b101:
            if(!w) return REG_CH;
            return (size == SIZE_16) ? REG_BP : REG_EBP;
        case 0b110:
            if(!w) return REG_DH;
            return (size == SIZE_16) ? REG_SI : REG_ESI;
        case 0b111:
            if(!w) return REG_BH;
            return (size == SIZE_16) ? REG_DI : REG_EDI;
    }

    return REG_UNKNOWN;
}

size_t decode_opcode_6x(uint8_t opcode_l, const uint8_t* ixns, INSTR* instr) {
    switch(opcode_l) {
        case 0b0000: // OP_PUSHA/OP_PUSHAD
        case 0b0001: // OP_POPA/OP_POPAD
            instr->opcode =
                (opcode_l & 0x1) ? OP_POPAD : OP_PUSHAD;
            return 1;
        case 0b0010: // BOUND
            break;
        case 0b0011: // ARPL
            break;
        case 0b1000:
        case 0b1010: // PUSH
            break;
        case 0b1001:
        case 0b1011: // IMUL
            break;
        case 0b1100:
        case 0b1101: // INS
            break;
        case 0b1110:
        case 0b1111: // OUTS
            break;
    }

    return 0;
}

size_t decode_opcode_9x(uint8_t opcode_l, const uint8_t* ixns, INSTR* instr) {
    switch(opcode_l) {
        case 0b0000: // NOP/XCHG EAX, EAX
            break;
        case 0b1000: // OP_CBW/OP_CWDE
        case 0b1001: // OP_CWD/OP_CDQ
            instr->opcode =
                (opcode_l & 0x1) ? OP_CDQ : OP_CWDE;
            return 1;
        case 0b1010: // CALL (direct, other seg)
            break;
        case 0b1011: // WAIT
            break;
        case 0b1100: // PUSHF/PUSHFD
        case 0b1101: // POPF/POPFD
            break;
        case 0b1110: // SAHF
        case 0b1111: // LAHF
            break;
    }

    return 0;
}

size_t decode_opcode_basic(uint8_t flags, const uint8_t* ixns, INSTR* instr) {
    // Parse flags
    const bool imm = flags & 0x4;
    const bool d = flags & 0x2;
    const bool w = flags & 0x1;

    if(imm) {
        if(w) {
            instr->operand1 = *ixns;
            return 2;
        }

        // TODO: support SIZE_16
        instr->operand1 = *(uint32_t*)ixns;
        return 5;
    }

    const uint8_t modrm = *ixns;
    const uint8_t mod = modrm >> 6;
    const uint8_t rm  = modrm & 0x3;
    const uint8_t reg = (modrm & 0x38) >> 3;

    switch(mod) {
        case 0b00:
            break;
        case 0b01:
            break;
        case 0b10:
            break;
        case 0b11:
            instr->operand1 = regbits_to_enum_w(reg, w, SIZE_32);
            instr->operand2 = regbits_to_enum_w(rm, w, SIZE_32);
            break;
    }

    // If direction bit is set, swap the operands
    if(d) {
        uint32_t tmp = instr->operand1;
        instr->operand1 = instr->operand2;
        instr->operand2 = tmp;
    }

    return 2; // These are all 2 byte instructions
}


// Decode a single instruction from the byte stream ixns
// into the INSTR structure passed by reference
// This assumes that instr has been initialized to all zeroes
size_t decode(const uint8_t* ixns, INSTR* instr) {
    if(!instr) abort();

    const uint8_t ixn = *ixns;

    const uint8_t opcode_h = ixn >> 4;
    const uint8_t opcode_l = ixn & 0xF;
    // Parse out prefix bytes
    // 2x
        // Prefix byte (CS/ES seg override)
    // 3x
        // Prefix byte (DS/SS seg override)
    // 6x
        // Prefix byte (address size)
        // Prefix byte (operand size)
        // Prefix byte (FS/GS seg override)
    // Fx
        // Prefix byte (LOCK)
        // Prefix byte (REP/REPE/REPNE)

    // Parse the opcode and following bytes
    switch(opcode_h) {
        case 0b0000:
            // All 2 or 3 byte opcodes
            if(opcode_l == 0xF) break;

            // OP_ADD
            // OP_OR
            instr->opcode = (opcode_l & 0x8) ? OP_OR : OP_ADD;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);
        case 0b0001:
            // ADC
            // SBB
            instr->opcode = (opcode_l & 0x8) ? OP_SBB : OP_ADC;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);
        case 0b0010:
            // DAA
            // DAS
            if(opcode_l == 0x7 || opcode_l == 0xF) break;

            // AND
            // SUB
            instr->opcode = (opcode_l & 0x8) ? OP_SUB : OP_AND;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);
        case 0b0011:
            // AAA
            // AAS
            if(opcode_l == 0x7 || opcode_l == 0xF) break;

            // CMP
            // XOR
            instr->opcode = (opcode_l & 0x8) ? OP_CMP : OP_XOR;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);

        case 0b0100:
            // DEC (alternate)
            // INC (alternate)
            break;
        case 0b0101:
            // POP (alternate)
            // PUSH (alternate)
            break;
        case 0b0110:
            return decode_opcode_6x(opcode_l, ixns + 1, instr);
        case 0b0111:
            // Jcc
            break;
        case 0b1000:
            // ADC (immediate)
            // OP_ADD (immediate)
            // AND (immediate)
            // CMP (immediate)
            // LEA
            // MOV
            // MOV (seg register)
            // OP_OR (immediate)
            // POP
            // SBB (immediate)
            // SUB (immediate)
            // TEST
            // XCHG
            // XOR (immediate)
            break;
        case 0b1001:
        case 0b1010:
            // CMPS/CMPSB/CMPSW/CMPSD
            // LODS/LODSB/LODSW/LODSD
            // MOV EAX (to and from)
            // MOVS/MOVSB/MOVSW/MOVSD
            // SCAS/SCASB/SCASW/SCASD
            // STOS/STOSB/STOSW/STOSD
            // TEST EAX, immediate
            break;
        case 0b1011:
            // MOV (alternate)
            break;
        case 0b1100:
            // INT n
            // INT 3
            // INTO
            // IRET/IRETD
            // LDS
            // LEAVE
            // LES
            // MOV (immediate)
            // RCL (immediate)
            // RCR (immediate)
            // RET
            // ROL (immediate)
            // ROR (immediate)
            // SAR (immediate)
            // SHL (immediate)
            // SHR (immediate)
            break;
        case 0b1101:
            // AAD
            // AAM
            // RCL
            // RCR
            // ROL
            // ROR
            // SAR
            // SHL
            // SHR
            // XLAT/XLATB
            break;
        case 0b1110:
            // CALL (direct)
            // IN
            // JCXZ/JECXZ
            // JMP (direct)
            // LOOP
            // LOOPZ/LOOPE
            // LOOPNZ/LOOPNE
            // OUT
            break;
        case 0b1111:
            // CALL (indirect)
            // CLC
            // CLD
            // CLI
            // CMC
            // DEC
            // DIV
            // HALT
            // IDIV
            // IMUL EAX
            // INC
            // JMP (indirect)
            // LOCK
            // MUL
            // NEG
            // NOT
            // PUSH
            // STC
            // STD
            // STI
            // TEST (immediate)
            break;
    }

    return 0;
}

void print_instr(INSTR* instr) {
    if(!instr) abort();

    switch(instr->opcode) {
        case OP_PUSHA:
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
        if(ixn_size < 1) {
            printf("Could not decode instruction starting with: %hhx\n", *ixns);
            return EXIT_FAILURE;
        }
        print_instr(&instr);

        ixns += ixn_size;
    }

    return EXIT_SUCCESS;
}
