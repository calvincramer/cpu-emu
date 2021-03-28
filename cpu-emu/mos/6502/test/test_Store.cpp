#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Store functions
void store_common_zero_page(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    storeReg = 0x42;    // Value in register to store
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[(u8) (0x50 + offsetVal)] == storeReg);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_START] = storeInst;
        cpu[RESET_START + 1] = 0x50;
        storeReg = 0x42;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu[(u8) (0x50 + offsetVal)] == storeReg);
    }
}

void store_common_absolute(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x50;
    cpu[RESET_START + 2] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    storeReg = 0x42;    // Value in register to store
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x5050 + offsetVal] == storeReg);
}

void store_indexed_indirect(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles) {
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    storeReg = 0x67;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x1234] == storeReg);
}

void store_indirect_indexed(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles) {
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    storeReg = 0x67;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x1239] == storeReg);
}


// STA
TEST_F(STA, ZeroPage)        { store_common_zero_page(cpu, STA_ZPG, cpu.A, 3); }
TEST_F(STA, ZeroPageX)       { store_common_zero_page(cpu, STA_ZPX, cpu.A, 4, &cpu.X, 0xFF); }
TEST_F(STA, Absolute)        { store_common_absolute (cpu, STA_ABS, cpu.A, 4); }
TEST_F(STA, AbsoluteX)       { store_common_absolute (cpu, STA_ABX, cpu.A, 5, &cpu.X, 0xFF); }
TEST_F(STA, AbsoluteY)       { store_common_absolute (cpu, STA_ABY, cpu.A, 5, &cpu.Y, 0x45); }
TEST_F(STA, IndexedIndirect) { store_indexed_indirect(cpu, STA_IDX, cpu.A, 6); }
TEST_F(STA, IndirectIndexed) { store_indirect_indexed(cpu, STA_IDY, cpu.A, 6); }


// STX
TEST_F(STX, ZeroPage)   { store_common_zero_page(cpu, STX_ZPG, cpu.X, 3); }
TEST_F(STX, ZeroPageY)  { store_common_zero_page(cpu, STX_ZPY, cpu.X, 4, &cpu.Y, 0xFF); }
TEST_F(STX, Absolute)   { store_common_absolute (cpu, STX_ABS, cpu.X, 4); }


// STY
TEST_F(STY, ZeroPage)   { store_common_zero_page(cpu, STY_ZPG, cpu.Y, 3); }
TEST_F(STY, ZeroPageX)  { store_common_zero_page(cpu, STY_ZPX, cpu.Y, 4, &cpu.X, 0xFF); }
TEST_F(STY, Absolute)   { store_common_absolute (cpu, STY_ABS, cpu.Y, 4); }
