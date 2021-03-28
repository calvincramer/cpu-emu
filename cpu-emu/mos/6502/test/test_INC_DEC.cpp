#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Increment / decrement
void inc_dec_implied(CPU& cpu, u8 inst, u8& incDecReg, u8 startingVal, s8 offset, u1 zeroFlagExpect, u1 negativeFlagExpect) {
    cpu[RESET_START] = inst;
    incDecReg = startingVal;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(incDecReg == (u8) (startingVal + offset));
    ASSERT_TRUE(cpu.get_flag_z() == zeroFlagExpect);
    ASSERT_TRUE(cpu.get_flag_n() == negativeFlagExpect);
    cpu.reset();
}

// Zero and Negative flags not tested for INC and DEC, but covered under INX, INY, DEX, DEY
TEST_F(INC_DEC, Increment) {
    // Zero page
    cpu[RESET_START] = INC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x41;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x12] == 0x42);

    // Zero page x
    cpu.reset();
    cpu[RESET_START] = INC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu.X = 0x10;
    cpu[0x22] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x22] == 0x42);

    // Zero page x wrap
    cpu.reset();
    cpu[RESET_START] = INC_ZPX;
    cpu[RESET_START + 1] = 0xF5;
    cpu.X = 0x10;
    cpu[0x05] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x05] == 0x42);

    // Absolute
    cpu.reset();
    cpu[RESET_START] = INC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x42);

    // Absolute x
    cpu.reset();
    cpu[RESET_START] = INC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x6;
    cpu[0x123A] = 0x41;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x123A] == 0x42);
}
TEST_F(INC_DEC, IncrementX) {
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0xFF, 1, F_ZERO, F_NON_NEG);       // Zero
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0x41, 1, F_NON_ZERO, F_NON_NEG);   // Positive
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0xF0, 1, F_NON_ZERO, F_NEG);       // Negative
}
TEST_F(INC_DEC, IncrementY) {
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0xFF, 1, F_ZERO, F_NON_NEG);       // Zero
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0x41, 1, F_NON_ZERO, F_NON_NEG);   // Positive
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0xF0, 1, F_NON_ZERO, F_NEG);       // Negative
}
TEST_F(INC_DEC, Decrement) {
    // Zero page
    cpu[RESET_START] = DEC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x43;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x12] == 0x42);

    // Zero page x
    cpu.reset();
    cpu[RESET_START] = DEC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu.X = 0x10;
    cpu[0x22] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x22] == 0x42);

    // Zero page x wrap
    cpu.reset();
    cpu[RESET_START] = DEC_ZPX;
    cpu[RESET_START + 1] = 0xF5;
    cpu.X = 0x10;
    cpu[0x05] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x05] == 0x42);

    // Absolute
    cpu.reset();
    cpu[RESET_START] = DEC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x42);

    // Absolute x
    cpu.reset();
    cpu[RESET_START] = DEC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x6;
    cpu[0x123A] = 0x43;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x123A] == 0x42);
}
TEST_F(INC_DEC, DecrementX) {
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0x1,  -1, F_ZERO, F_NON_NEG);      // Zero
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0x43, -1, F_NON_ZERO, F_NON_NEG);  // Positive
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0xF0, -1, F_NON_ZERO, F_NEG);      // Negative
}
TEST_F(INC_DEC, DecrementY) {
    inc_dec_implied(cpu, DEY_IMP, cpu.Y, 0x1,  -1, F_ZERO, F_NON_NEG);      // Zero
    inc_dec_implied(cpu, DEY_IMP, cpu.Y, 0x43, -1, F_NON_ZERO, F_NON_NEG);  // Positive
    inc_dec_implied(cpu, DEY_IMP, cpu.Y, 0xF0, -1, F_NON_ZERO, F_NEG);      // Negative
}
