#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// LSR
TEST_F(LSR, Accumulator) {
    // No carry
    cpu[RESET_START] = LSR_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x04);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Carry
    cpu.reset();
    cpu[RESET_START] = LSR_ACC;
    cpu.A = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x7F);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = LSR_ACC;
    cpu.A = 0x01;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
}
// Assume flags are set correctly now, only test shift is correct
TEST_F(LSR, ZeroPage) {
    cpu[RESET_START] = LSR_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0x12;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x09);
}
TEST_F(LSR, ZeroPageX) {
    cpu[RESET_START] = LSR_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x09);
}
TEST_F(LSR, Absolute) {
    cpu[RESET_START] = LSR_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x09);
}
TEST_F(LSR, AbsoluteX) {
    cpu[RESET_START] = LSR_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0x12;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x09);
}
