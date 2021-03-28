#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// ADC
TEST_F(ADC, Immediate) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x42);
}
TEST_F(ADC, ImmediateZeroFlag) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0xEE;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
}
TEST_F(ADC, ImmediateNegativeFlag) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0xE0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF2);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(ADC, ImmediateCarryFlag) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0xEF;
    cpu.A = 0x20;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0F);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
}
TEST_F(ADC, ImmediateWithCarryEnabled) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x43);
}
TEST_F(ADC, ZeroPage) {
    cpu[RESET_START] = ADC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, ZeroPageX) {
    cpu[RESET_START] = ADC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x22] = 0x40;
    cpu.A = 0x30;
    cpu.X = 0x10;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, Absolute) {
    cpu[RESET_START] = ADC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, AbsoluteX) {
    // No page cross
    cpu[RESET_START] = ADC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x10;
    cpu[0x1244] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ADC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0xFF;
    cpu[0x1333] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, AbsoluteY) {
    // No page cross
    cpu[RESET_START] = ADC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0x11;
    cpu[0x1245] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ADC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0xF0;
    cpu[0x1324] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, IndirectX) {
    cpu[RESET_START] = ADC_IDX;
    cpu[RESET_START + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;
    cpu.A = 0x30;
    cpu[0x1234] = 0x40;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, IndirectY) {
    // No page cross
    cpu[RESET_START] = ADC_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x30;
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x85);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ADC_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x30;
    cpu[0x1326] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x85);
}
