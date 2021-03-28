#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// CMP
TEST_F(CMP, Immediate) {
    // Equal
    cpu[RESET_START] = CMP_IMM;
    cpu[RESET_START + 1] = 0x42;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Greater
    cpu.reset();
    cpu[RESET_START] = CMP_IMM;
    cpu[RESET_START + 1] = 0x21;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);

    // Less
    cpu.reset();
    cpu[RESET_START] = CMP_IMM;
    cpu[RESET_START + 1] = 0x42;
    cpu.A = 0x21;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Just test for one result of comparison now
TEST_F(CMP, ZeroPage) {
    cpu[RESET_START] = CMP_ZPG;
    cpu[RESET_START + 1] = 0x51;
    cpu[0x51] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, ZeroPageX) {
    cpu[RESET_START] = CMP_ZPX;
    cpu[RESET_START + 1] = 0x51;
    cpu.X = 0x10;
    cpu[0x61] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, Absolute) {
    cpu[RESET_START] = CMP_ABS;
    cpu[RESET_START + 1] = 0x51;
    cpu[RESET_START + 2] = 0x74;
    cpu[0x7451] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, AbsoluteX) {
    cpu[RESET_START] = CMP_ABX;
    cpu[RESET_START + 1] = 0x51;
    cpu[RESET_START + 2] = 0x74;
    cpu.X = 0x10;
    cpu[0x7461] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, AbsoluteY) {
    cpu[RESET_START] = CMP_ABY;
    cpu[RESET_START + 1] = 0x51;
    cpu[RESET_START + 2] = 0x74;
    cpu.Y = 0x10;
    cpu[0x7461] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, IndirectX) {
    cpu[RESET_START] = CMP_IDX;
    cpu[RESET_START + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;
    cpu.A = 0x42;
    cpu[0x1234] = 0x31;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, IndirectY) {
    // No page cross
    cpu[RESET_START] = CMP_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x42;
    cpu[0x1239] = 0x31;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
// Since CPX and CPY are so similar to CMP, just test a little
TEST_F(CMP, CPX_ImmediateSimple) {
    cpu[RESET_START] = CPX_IMM;
    cpu[RESET_START + 1] = 0x21;
    cpu.X = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, CPY_ImmediateSimple) {
    cpu[RESET_START] = CPY_IMM;
    cpu[RESET_START + 1] = 0x21;
    cpu.Y = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
