#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Tests from http://www.obelisk.me.uk/6502/algorithms.html
TEST_F(OBELISK_TESTS, SimpleMemoryOperationsClear16bits) {
    cpu[RESET_START] = LDA_IMM;
    cpu[RESET_START + 1] = 0;
    cpu[RESET_START + 2] = STA_ZPG;
    cpu[RESET_START + 3] = 0x10;
    cpu[RESET_START + 4] = STA_ZPG;
    cpu[RESET_START + 5] = 0x11;

    for (u16 addr = 0x0; addr < 0x100; addr++) {
        cpu[addr] = 0xff;
    }

    ASSERT_TRUE(cpu.execute(2 + 3 + 3) == 2 + 3 + 3);
    ASSERT_TRUE(cpu[0x10] == 0);
    ASSERT_TRUE(cpu[0x11] == 0);
}
TEST_F(OBELISK_TESTS, SimpleMemoryOperationsClear32bits) {
    cpu[RESET_START] = LDA_IMM;
    cpu[RESET_START + 1] = 0;
    cpu[RESET_START + 2] = STA_ZPG;
    cpu[RESET_START + 3] = 0x10;
    cpu[RESET_START + 4] = STA_ZPG;
    cpu[RESET_START + 5] = 0x11;
    cpu[RESET_START + 6] = STA_ZPG;
    cpu[RESET_START + 7] = 0x12;
    cpu[RESET_START + 8] = STA_ZPG;
    cpu[RESET_START + 9] = 0x13;

    for (u16 addr = 0x0; addr < 0x100; addr++) {
        cpu[addr] = 0xff;
    }

    ASSERT_TRUE(cpu.execute(2 + 3 + 3 + 3 + 3) == 2 + 3 + 3 + 3 + 3);
    ASSERT_TRUE(cpu[0x10] == 0);
    ASSERT_TRUE(cpu[0x11] == 0);
    ASSERT_TRUE(cpu[0x12] == 0);
    ASSERT_TRUE(cpu[0x13] == 0);
}
TEST_F(OBELISK_TESTS, SimpleMemoryOperationsSetWord) {
    cpu[RESET_START] = LDA_IMM;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = STA_ZPG;
    cpu[RESET_START + 3] = 0x10;
    cpu[RESET_START + 4] = LDA_IMM;
    cpu[RESET_START + 5] = 0x12;
    cpu[RESET_START + 6] = STA_ZPG;
    cpu[RESET_START + 7] = 0x11;

    for (u16 addr = 0x0; addr < 0x100; addr++) {
        cpu[addr] = 0xff;
    }

    ASSERT_TRUE(cpu.execute(2 + 3 + 2 + 3) == 2 + 3 + 2 + 3);
    ASSERT_TRUE(cpu[0x10] == 0x34);
    ASSERT_TRUE(cpu[0x11] == 0x12);
}
TEST_F(OBELISK_TESTS, EOR_Complement) {
    cpu.A = 0xF0;
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0F);
}
TEST_F(OBELISK_TESTS, Shift16Left) {
    cpu[RESET_START] = ASL_ZPG;
    cpu[RESET_START + 1] = 0x10;
    cpu[RESET_START + 2] = ROL_ZPG;
    cpu[RESET_START + 3] = 0x11;

    cpu[0x10] = 0xF4;
    cpu[0x11] = 0xC0;
    ASSERT_TRUE(cpu.execute(5 + 5) == 5 + 5);
    ASSERT_TRUE(cpu[0x10] == 0xE8);
    ASSERT_TRUE(cpu[0x11] == 0x81);
}
TEST_F(OBELISK_TESTS, Shift16Right) {
    cpu[RESET_START] = LSR_ZPG;
    cpu[RESET_START + 1] = 0x11;
    cpu[RESET_START + 2] = ROR_ZPG;
    cpu[RESET_START + 3] = 0x10;

    cpu[0x10] = 0x00;
    cpu[0x11] = 0xFF;
    ASSERT_TRUE(cpu.execute(5 + 5) == 5 + 5);
    ASSERT_TRUE(cpu[0x10] == 0x80);
    ASSERT_TRUE(cpu[0x11] == 0x7F);
}
TEST_F(OBELISK_TESTS, Divide16bitSignedValueBy2) {
    cpu[0x11] = 0xF1; // F108 = -3832. -3832 / 2 = -1916 = F884
    cpu[0x10] = 0x08;
    cpu[RESET_START] = LDA_ZPG;
    cpu[RESET_START + 1] = 0x11;
    cpu[RESET_START + 2] = ASL_ACC;
    cpu[RESET_START + 3] = ROR_ZPG;
    cpu[RESET_START + 4] = 0x11;
    cpu[RESET_START + 5] = ROR_ZPG;
    cpu[RESET_START + 6] = 0x10;

    ASSERT_TRUE(cpu.execute(3 + 2 + 5 + 5) == 3 + 2 + 5 + 5);
    ASSERT_TRUE(cpu[0x11] == 0xF8);
    ASSERT_TRUE(cpu[0x10] == 0x84);
}
TEST_F(OBELISK_TESTS, 16BitBinaryAddition) {
    // 0x0FFF + 0x0FFF = 0x1FFE
    u16 a_addr = 0x10;
    u16 b_addr = 0x20;
    u16 res_addr = 0x30;
    cpu[a_addr + 1] = 0x0F;
    cpu[a_addr]     = 0xFF;
    cpu[b_addr + 1] = 0x0F;
    cpu[b_addr]     = 0xFF;

    cpu[RESET_START + 0] = CLC_IMP;
    cpu[RESET_START + 1] = LDA_ZPG;
    cpu[RESET_START + 2] = a_addr;
    cpu[RESET_START + 3] = ADC_ZPG;
    cpu[RESET_START + 4] = b_addr;
    cpu[RESET_START + 5] = STA_ZPG;
    cpu[RESET_START + 6] = res_addr;
    cpu[RESET_START + 7] = LDA_ZPG;
    cpu[RESET_START + 8] = a_addr + 1;  // Hard code addr, no offset just because
    cpu[RESET_START + 9] = ADC_ZPG;
    cpu[RESET_START + 10] = b_addr + 1;
    cpu[RESET_START + 11] = STA_ZPG;
    cpu[RESET_START + 12] = res_addr + 1;

    ASSERT_TRUE(cpu.execute(2 + (3 * 6)) == 2 + (3 * 6));
    ASSERT_TRUE(cpu[res_addr + 0] == 0xFE);
    ASSERT_TRUE(cpu[res_addr + 1] == 0x1F);
}
TEST_F(OBELISK_TESTS, 16BitBinarySubtraction) {
    // 0x8888 + 0x4FAA = 0x38DE
    u16 a_addr = 0x10;
    u16 b_addr = 0x20;
    u16 res_addr = 0x30;
    cpu[a_addr + 1] = 0x88;
    cpu[a_addr]     = 0x88;
    cpu[b_addr + 1] = 0x4F;
    cpu[b_addr]     = 0xAA;

    cpu[RESET_START + 0] = SEC_IMP;
    cpu[RESET_START + 1] = LDA_ZPG;
    cpu[RESET_START + 2] = a_addr;
    cpu[RESET_START + 3] = SBC_ZPG;
    cpu[RESET_START + 4] = b_addr;
    cpu[RESET_START + 5] = STA_ZPG;
    cpu[RESET_START + 6] = res_addr;
    cpu[RESET_START + 7] = LDA_ZPG;
    cpu[RESET_START + 8] = a_addr + 1;  // Hard code addr, no offset just because
    cpu[RESET_START + 9] = SBC_ZPG;
    cpu[RESET_START + 10] = b_addr + 1;
    cpu[RESET_START + 11] = STA_ZPG;
    cpu[RESET_START + 12] = res_addr + 1;

    ASSERT_TRUE(cpu.execute(2 + (3 * 6)) == 2 + (3 * 6));
    ASSERT_TRUE(cpu[res_addr + 0] == 0xDE);
    ASSERT_TRUE(cpu[res_addr + 1] == 0x38);
}
TEST_F(OBELISK_TESTS, TwosComplementNegation8Bit) {
    cpu.A = 0xF0;   // -16
    cpu[RESET_START + 0] = CLC_IMP;
    cpu[RESET_START + 1] = EOR_IMM;
    cpu[RESET_START + 2] = 0xFF;
    cpu[RESET_START + 3] = ADC_IMM;
    cpu[RESET_START + 4] = 0x1;

    ASSERT_TRUE(cpu.execute(2 + 2 + 2) == 6);
    ASSERT_TRUE(cpu.A == 0x10); // +16
}
TEST_F(OBELISK_TESTS, TwosComplementNegation16Bit) {
    u16 src = 0x10;
    u16 dst = 0x20;

    cpu[src + 1] = 0xF1;
    cpu[src + 0] = 0x11;    // 0xF111 = -3823 -> 3823 = 0x0EEF

    cpu[RESET_START + 0] = SEC_IMP;
    cpu[RESET_START + 1] = LDA_IMM;
    cpu[RESET_START + 2] = 0x0;
    cpu[RESET_START + 3] = SBC_ZPG;
    cpu[RESET_START + 4] = src;
    cpu[RESET_START + 5] = STA_ZPG;
    cpu[RESET_START + 6] = dst;
    cpu[RESET_START + 7] = LDA_IMM;
    cpu[RESET_START + 8] = 0x0;
    cpu[RESET_START + 9] = SBC_ZPG;
    cpu[RESET_START + 10]= src + 1;
    cpu[RESET_START + 11]= STA_ZPG;
    cpu[RESET_START + 12]= dst + 1;

    ASSERT_TRUE(cpu.execute(2 + 2 + 3 + 3 + 2 + 3 + 3) == 2 + 2 + 3 + 3 + 2 + 3 + 3);
    ASSERT_TRUE(cpu[dst + 1] == 0x0E);
    ASSERT_TRUE(cpu[dst + 0] == 0xEF);
}
TEST_F(OBELISK_TESTS, BCD_Addition_16Bit) {
    u16 a_addr = 0x10;
    u16 b_addr = 0x20;
    u16 res_addr = 0x30;

    cpu[a_addr + 1] = 0x38;
    cpu[a_addr]     = 0x52;
    cpu[b_addr + 1] = 0x47;
    cpu[b_addr]     = 0x87;

    // (BCD) 0x3852 + 0x4787 = 0x8639

    cpu[RESET_START + 0]  = SED_IMP;
    cpu[RESET_START + 1]  = CLC_IMP;
    cpu[RESET_START + 2]  = LDA_ZPG;
    cpu[RESET_START + 3]  = a_addr;
    cpu[RESET_START + 4]  = ADC_ZPG;
    cpu[RESET_START + 5]  = b_addr;
    cpu[RESET_START + 6]  = STA_ZPG;
    cpu[RESET_START + 7]  = res_addr;
    cpu[RESET_START + 8]  = LDA_ZPG;
    cpu[RESET_START + 9]  = a_addr + 1;
    cpu[RESET_START + 10] = ADC_ZPG;
    cpu[RESET_START + 11] = b_addr + 1;
    cpu[RESET_START + 12] = STA_ZPG;
    cpu[RESET_START + 13] = res_addr + 1;
    cpu[RESET_START + 14] = CLD_IMP;

    ASSERT_TRUE(cpu.execute(2 + 2 + 3 + 3 + 3 + 3 + 3 + 3 + 2) == 2 + 2 + 3 + 3 + 3 + 3 + 3 + 3 + 2);
    ASSERT_TRUE(cpu[res_addr + 1] == 0x86);
    ASSERT_TRUE(cpu[res_addr + 0] == 0x39);
}
TEST_F(OBELISK_TESTS, 16Bit_to_24BitBCD) {
    u16 val_addr = 0x10;
    u16 res_addr = 0x20;

    cpu[val_addr + 1] = 0x7E;
    cpu[val_addr + 0] = 0xDC;

    // 0x7EDC = 32476

    cpu[RESET_START + 0]  = LDA_IMM;
    cpu[RESET_START + 1]  = 0;
    cpu[RESET_START + 2]  = STA_ZPG;
    cpu[RESET_START + 3]  = res_addr;
    cpu[RESET_START + 4]  = STA_ZPG;
    cpu[RESET_START + 5]  = res_addr + 1;
    cpu[RESET_START + 6]  = STA_ZPG;
    cpu[RESET_START + 7]  = res_addr + 2;
    cpu[RESET_START + 8]  = LDX_IMM;
    cpu[RESET_START + 9]  = 16;
    cpu[RESET_START + 10] = SED_IMP;
    cpu[RESET_START + 11] = ASL_ZPG;    // Label: _LOOP
    cpu[RESET_START + 12] = val_addr;
    cpu[RESET_START + 13] = ROL_ZPG;
    cpu[RESET_START + 14] = val_addr + 1;
    cpu[RESET_START + 15] = LDA_ZPG;
    cpu[RESET_START + 16] = res_addr;
    cpu[RESET_START + 17] = ADC_ZPG;
    cpu[RESET_START + 18] = res_addr;
    cpu[RESET_START + 19] = STA_ZPG;
    cpu[RESET_START + 20] = res_addr;
    cpu[RESET_START + 21] = LDA_ZPG;
    cpu[RESET_START + 22] = res_addr + 1;
    cpu[RESET_START + 23] = ADC_ZPG;
    cpu[RESET_START + 24] = res_addr + 1;
    cpu[RESET_START + 25] = STA_ZPG;
    cpu[RESET_START + 26] = res_addr + 1;
    cpu[RESET_START + 27] = LDA_ZPG;
    cpu[RESET_START + 28] = res_addr + 2;
    cpu[RESET_START + 29] = ADC_ZPG;
    cpu[RESET_START + 30] = res_addr + 2;
    cpu[RESET_START + 31] = STA_ZPG;
    cpu[RESET_START + 32] = res_addr + 2;
    cpu[RESET_START + 33] = DEX_IMP;
    cpu[RESET_START + 34] = BNE_REL;    // to _LOOP
    cpu[RESET_START + 35] = 0xE9;       // -23 bytes
    cpu[RESET_START + 36] = CLD_IMP;
    cpu[RESET_START + 37] = INVALID_INSTRUCTION;

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 37);    // should stop at first invalid instruction
    ASSERT_TRUE(cpu[res_addr + 2] == 0x03);
    ASSERT_TRUE(cpu[res_addr + 1] == 0x24);
    ASSERT_TRUE(cpu[res_addr + 0] == 0x76);
}
TEST_F(OBELISK_TESTS, Inc16) {
    u16 mem_addr = 0x50;

    cpu[mem_addr + 0] = 0xFF;
    cpu[mem_addr + 1] = 0x41;

    cpu[RESET_START + 0] = INC_ZPG;
    cpu[RESET_START + 1] = mem_addr;
    cpu[RESET_START + 2] = BNE_REL;
    cpu[RESET_START + 3] = 4;                       // Branch to 'Done'
    cpu[RESET_START + 4] = INC_ZPG;
    cpu[RESET_START + 5] = mem_addr + 1;
    cpu[RESET_START + 6] = INVALID_INSTRUCTION;     // Done

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 6);
    ASSERT_TRUE(cpu[mem_addr + 0] == 0x00);
    ASSERT_TRUE(cpu[mem_addr + 1] == 0x42);
}
TEST_F(OBELISK_TESTS, Dec16) {
    u16 mem_addr = 0x50;

    cpu[mem_addr + 0] = 0x00;
    cpu[mem_addr + 1] = 0x42;

    cpu[RESET_START + 0] = LDA_ZPG;
    cpu[RESET_START + 1] = mem_addr;
    cpu[RESET_START + 2] = BNE_REL;
    cpu[RESET_START + 3] = 4;                       // Branch to 'Skip'
    cpu[RESET_START + 4] = DEC_ZPG;
    cpu[RESET_START + 5] = mem_addr + 1;
    cpu[RESET_START + 6] = DEC_ZPG;                 // Skip
    cpu[RESET_START + 7] = mem_addr;
    cpu[RESET_START + 8] = INVALID_INSTRUCTION;

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 8);
    ASSERT_TRUE(cpu[mem_addr + 0] == 0xFF);
    ASSERT_TRUE(cpu[mem_addr + 1] == 0x41);
}
TEST_F(OBELISK_TESTS, Move256) {
    u16 src = 0x1000;
    u16 dst = 0x2000;
    u8 length = 0xFF;

    for (u16 i = src; i < length; i++) {
        cpu[src + i] = i;
    }

    cpu[RESET_START +  0] = LDX_IMM;
    cpu[RESET_START +  1] = 0;
    cpu[RESET_START +  2] = LDA_ABX;         // Loop
    cpu[RESET_START +  3] = lowByte(src);
    cpu[RESET_START +  4] = highByte(src);
    cpu[RESET_START +  5] = STA_ABX;
    cpu[RESET_START +  6] = lowByte(dst);
    cpu[RESET_START +  7] = highByte(dst);
    cpu[RESET_START +  8] = INX_IMP;
    cpu[RESET_START +  9] = CPX_IMM;
    cpu[RESET_START + 10] = length;
    cpu[RESET_START + 11] = BNE_REL;         // Branch to Loop (-9 bytes)
    cpu[RESET_START + 12] = 0xF7;
    cpu[RESET_START + 13] = INVALID_INSTRUCTION;

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 13);
    for (u16 i = src; i < length; i++) {
        ASSERT_TRUE(cpu[dst + i] == i);
    }
}
TEST_F(OBELISK_TESTS, Move256Reverse) {
    u16 src = 0x1000;
    u16 dst = 0x2000;
    u8 length = 0xFF;

    for (u16 i = src; i < length; i++) {
        cpu[src + i] = i;
    }

    cpu[RESET_START +  0] = LDX_IMM;
    cpu[RESET_START +  1] = length;
    cpu[RESET_START +  2] = DEX_IMP;         // Loop
    cpu[RESET_START +  3] = LDA_ABX;
    cpu[RESET_START +  4] = lowByte(src);
    cpu[RESET_START +  5] = highByte(src);
    cpu[RESET_START +  6] = STA_ABX;
    cpu[RESET_START +  7] = lowByte(dst);
    cpu[RESET_START +  8] = highByte(dst);
    cpu[RESET_START +  9] = CPX_IMM;
    cpu[RESET_START + 10] = 0;
    cpu[RESET_START + 11] = BNE_REL;         // Branch to Loop (-9 bytes)
    cpu[RESET_START + 12] = 0xF7;
    cpu[RESET_START + 13] = INVALID_INSTRUCTION;

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 13);
    for (u16 i = src; i < length; i++) {
        ASSERT_TRUE(cpu[dst + i] == length - i);
    }
}
TEST_F(OBELISK_TESTS, Move128Reverse) {
    u16 src = 0x1000;
    u16 dst = 0x2000;
    u8 length = 0x7F;

    for (u16 i = src; i < length; i++) {
        cpu[src + i] = i;
    }

    cpu[RESET_START +  0] = LDX_IMM;
    cpu[RESET_START +  1] = length - 1;
    cpu[RESET_START +  2] = LDA_ABX;        // Loop
    cpu[RESET_START +  3] = lowByte(src);
    cpu[RESET_START +  4] = highByte(src);
    cpu[RESET_START +  5] = STA_ABX;
    cpu[RESET_START +  6] = lowByte(dst);
    cpu[RESET_START +  7] = highByte(dst);
    cpu[RESET_START +  8] = DEX_IMP;
    cpu[RESET_START +  9] = BPL_REL;         // Branch to Loop (-7 bytes)
    cpu[RESET_START + 10] = 0xF9;
    cpu[RESET_START + 11] = INVALID_INSTRUCTION;

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 11);
    for (u16 i = src; i < length; i++) {
        ASSERT_TRUE(cpu[dst + i] == length - i);
    }
}
TEST_F(OBELISK_TESTS, MoveGeneric) {
    u16 src = 0x1234;
    u16 dst = 0x5678;
    u16 length = 0x555;

    for (u16 i = src; i < length; i++) {
        cpu[src + i] = i % 0x100;
    }

    u16 src_ptr = 0x10;
    cpu[src_ptr + 0] = lowByte(src);
    cpu[src_ptr + 1] = highByte(src);

    u16 dst_ptr = 0x12;
    cpu[dst_ptr + 0] = lowByte(dst);
    cpu[dst_ptr + 1] = highByte(dst);


    cpu[RESET_START +  0] = LDY_IMM;    // MOVFWD
    cpu[RESET_START +  1] = 0;
    cpu[RESET_START +  2] = LDX_IMM;
    cpu[RESET_START +  3] = highByte(length);
    cpu[RESET_START +  4] = BEQ_REL;    // Branch to FRAG (+16)
    cpu[RESET_START +  5] = 0x10;
    cpu[RESET_START +  6] = LDA_IDY;    // PAGE
    cpu[RESET_START +  7] = src_ptr;
    cpu[RESET_START +  8] = STA_IDY;
    cpu[RESET_START +  9] = dst_ptr;
    cpu[RESET_START + 10] = INY_IMP;
    cpu[RESET_START + 11] = BNE_REL;    // Branch to PAGE (-5)
    cpu[RESET_START + 12] = 0xFB;
    cpu[RESET_START + 13] = INC_ZPG;
    cpu[RESET_START + 14] = src_ptr + 1;
    cpu[RESET_START + 15] = INC_ZPG;
    cpu[RESET_START + 16] = dst_ptr + 1;
    cpu[RESET_START + 17] = DEX_IMP;
    cpu[RESET_START + 18] = BNE_REL;    // Branch to PAGE (-12)
    cpu[RESET_START + 19] = 0xF4;
    cpu[RESET_START + 20] = CPY_IMM;    // FRAG
    cpu[RESET_START + 21] = lowByte(length);
    cpu[RESET_START + 22] = BEQ_REL;    // Branch to DONE (+9)
    cpu[RESET_START + 23] = 0x9;
    cpu[RESET_START + 24] = LDA_IDY;
    cpu[RESET_START + 25] = src_ptr;
    cpu[RESET_START + 26] = STA_IDY;
    cpu[RESET_START + 27] = dst_ptr;
    cpu[RESET_START + 28] = INY_IMP;
    cpu[RESET_START + 29] = BNE_REL;    // Branch to FRAG (-9)
    cpu[RESET_START + 30] = 0xF7;
    cpu[RESET_START + 31] = INVALID_INSTRUCTION;    // DONE

    ASSERT_TRUE(cpu.execute(0, true) == -1);
    ASSERT_TRUE(cpu.PC == RESET_START + 31);
    for (u16 i = src; i < length; i++) {
        ASSERT_TRUE(cpu[dst + i] == i % 0x100);
    }
}
