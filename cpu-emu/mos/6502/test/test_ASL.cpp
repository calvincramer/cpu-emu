#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// ASL
TEST_F(ASL, Accumulator) {
    // No carry
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x10);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Carry
    cpu.reset();
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFE);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0x80;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0x40;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Assume flags are set correctly now, only test shift is correct
TEST_F(ASL, ZeroPage) {
    cpu[RESET_START] = ASL_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0x12;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x24);
}
TEST_F(ASL, ZeroPageX) {
    cpu[RESET_START] = ASL_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x24);
}
TEST_F(ASL, Absolute) {
    cpu[RESET_START] = ASL_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x24);
}
TEST_F(ASL, AbsoluteX) {
    cpu[RESET_START] = ASL_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0x12;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x24);
}
