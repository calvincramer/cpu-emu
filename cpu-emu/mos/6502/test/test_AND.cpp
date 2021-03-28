#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// And
void and_common_zero_page(CPU& cpu, u8 andInst, s8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_START] = andInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu.A = 0x42;
    cpu[(u8) (0x50 + offsetVal)] = 0xF0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0x40);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_START] = andInst;
        cpu[RESET_START + 1] = 0x50;
        cpu.A = 0x42;
        cpu[(u8) (0x50 + offsetVal)] = 0x0F;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0x02);
    }
}

void and_common_absolute(CPU& cpu, u8 andInst, s32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_START] = andInst;
    cpu[RESET_START + 1] = 0x68;
    cpu[RESET_START + 2] = 0x24;
    cpu.A = 0x42;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0xF0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0x40);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_START] = andInst;
        cpu[RESET_START + 1] = 0x68;
        cpu[RESET_START + 2] = 0x24;
        cpu.A = 0x42;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0xF0;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0x40);
    }
}

TEST_F(AND, Immediate) {
    // Zero
    cpu[RESET_START] = AND_IMM;
    cpu[RESET_START + 1] = 0x0;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = AND_IMM;
    cpu[RESET_START + 1] = 0x3C;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x30);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = AND_IMM;
    cpu[RESET_START + 1] = 0x8F;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(AND, ZeroPage)   { and_common_zero_page(cpu, AND_ZPG, 3); }
TEST_F(AND, ZeroPageX)  { and_common_zero_page(cpu, AND_ZPX, 4, &cpu.X, 0x20); }
TEST_F(AND, Absolute)   { and_common_absolute (cpu, AND_ABS, 4); }
TEST_F(AND, AbsoluteX)  { and_common_absolute (cpu, AND_ABX, 4, &cpu.X, 0x10); }
TEST_F(AND, AbsoluteY)  { and_common_absolute (cpu, AND_ABY, 4, &cpu.Y, 0x10);}
TEST_F(AND, IndirectX)  {
    cpu[RESET_START] = AND_IDX;
    cpu.A = 0x12;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0xF0;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(AND, IndirectY)  {
    // No page cross
    cpu[RESET_START] = AND_IDY;
    cpu.A = 0x12;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x0F;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x02);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = AND_IDY;
    cpu.A = 0x12;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0xF2
    cpu[0x1326] = 0xF0;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
