#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// ROL
TEST_F(ROL, Accumulator) {
    // No rotate bit
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x10);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Rotate bit
    cpu.reset();
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xE0);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x40;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Assume flags are set correctly now, only test rotate is correct
TEST_F(ROL, ZeroPage) {
    cpu[RESET_START] = ROL_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0xAA;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x54);
}
TEST_F(ROL, ZeroPageX) {
    cpu[RESET_START] = ROL_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0xAA;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x54);
}
TEST_F(ROL, Absolute) {
    cpu[RESET_START] = ROL_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0xAA;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x54);
}
TEST_F(ROL, AbsoluteX) {
    cpu[RESET_START] = ROL_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0xAA;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x54);
}
TEST_F(ROL, RotateWithCarrySet) {
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x18;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x31);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);
}
