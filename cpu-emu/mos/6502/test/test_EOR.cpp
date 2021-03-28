#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Eor (exclusive or)
void eor_common_zero_page(CPU& cpu, u8 eorInst, s8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_START] = eorInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu.A = 0xF0;
    cpu[(u8) (0x50 + offsetVal)] = 0x55;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_START] = eorInst;
        cpu[RESET_START + 1] = 0x50;
        cpu.A = 0xF0;
        cpu[(u8) (0x50 + offsetVal)] = 0x55;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0xA5);
    }
}

void eor_common_absolute(CPU& cpu, u8 eorInst, s32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_START] = eorInst;
    cpu[RESET_START + 1] = 0x68;
    cpu[RESET_START + 2] = 0x24;
    cpu.A = 0xF0;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0x55;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_START] = eorInst;
        cpu[RESET_START + 1] = 0x68;
        cpu[RESET_START + 2] = 0x24;
        cpu.A = 0xF0;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0x55;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0xA5);
    }
}

TEST_F(EOR, Immediate) {
    // Zero
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0xF0;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0x55;
    cpu.A = 0x0F;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x5A);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0x55;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xA5);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(EOR, ZeroPage)   { eor_common_zero_page(cpu, EOR_ZPG, 3); }
TEST_F(EOR, ZeroPageX)  { eor_common_zero_page(cpu, EOR_ZPX, 4, &cpu.X, 0x20); }
TEST_F(EOR, Absolute)   { eor_common_absolute (cpu, EOR_ABS, 4); }
TEST_F(EOR, AbsoluteX)  { eor_common_absolute (cpu, EOR_ABX, 4, &cpu.X, 0x10); }
TEST_F(EOR, AbsoluteY)  { eor_common_absolute (cpu, EOR_ABY, 4, &cpu.Y, 0x10);}
TEST_F(EOR, IndirectX)  {
    cpu[RESET_START] = EOR_IDX;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xA5);
}
TEST_F(EOR, IndirectY)  {
    // No page cross
    cpu[RESET_START] = EOR_IDY;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = EOR_IDY;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0xF2
    cpu[0x1326] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xA5);
}
