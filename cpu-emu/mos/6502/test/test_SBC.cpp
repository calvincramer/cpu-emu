#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// SBC
TEST_F(SBC, Immediate) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1E);
}
TEST_F(SBC, ImmediateZeroFlag) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x12;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
}
TEST_F(SBC, ImmediateNegativeFlag) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x13;
    cpu.A = 0x12;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(SBC, ImmediateCarryFlag) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x13;
    cpu.A = 0x12;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);    // Clear flag means carry
}
TEST_F(SBC, ImmediateWithCarryEnabled) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1E);
}
TEST_F(SBC, ZeroPage) {
    cpu[RESET_START] = SBC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x40;
    cpu.A = 0x50;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, ZeroPageX) {
    cpu[RESET_START] = SBC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x22] = 0x40;
    cpu.A = 0x50;
    cpu.X = 0x10;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, Absolute) {
    cpu[RESET_START] = SBC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x40;
    cpu.A = 0x50;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, AbsoluteX) {
    // No page cross
    cpu[RESET_START] = SBC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x10;
    cpu[0x1244] = 0x40;
    cpu.A = 0x50;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = SBC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0xFF;
    cpu[0x1333] = 0x40;
    cpu.A = 0x50;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, AbsoluteY) {
    // No page cross
    cpu[RESET_START] = SBC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0x11;
    cpu[0x1245] = 0x40;
    cpu.A = 0x50;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = SBC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0xF0;
    cpu[0x1324] = 0x40;
    cpu.A = 0x50;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, IndirectX) {
    cpu[RESET_START] = SBC_IDX;
    cpu[RESET_START + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;
    cpu.A = 0x50;
    cpu[0x1234] = 0x40;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, IndirectY) {
    // No page cross
    cpu[RESET_START] = SBC_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x50;
    cpu[0x1239] = 0x40;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = SBC_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x50;
    cpu[0x1326] = 0x40;
    cpu.set_flag_c(1);  // carry high to not subtract one
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
