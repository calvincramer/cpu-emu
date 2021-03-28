#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// ROR
TEST_F(ROR, Accumulator) {
    // No rotate bit
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x04);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Rotate bit
    cpu.reset();
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0xF1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF8);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0x01;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Assume flags are set correctly now, only test rotate is correct
TEST_F(ROR, ZeroPage) {
    cpu[RESET_START] = ROR_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0xE1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0xF0);
}
TEST_F(ROR, ZeroPageX) {
    cpu[RESET_START] = ROR_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu.set_flag_c(1);
    cpu[0x52] = 0xE1;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0xF0);
}
TEST_F(ROR, Absolute) {
    cpu[RESET_START] = ROR_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0xE1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0xF0);
}
TEST_F(ROR, AbsoluteX) {
    cpu[RESET_START] = ROR_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0xE1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0xF0);
}
