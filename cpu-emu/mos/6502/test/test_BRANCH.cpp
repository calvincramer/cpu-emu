#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Branching instructions
TEST_F(BRANCH, BCC_BranchFail) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.PC == start + 2);
}
TEST_F(BRANCH, BCC_BranchPassSamePage) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BCC_BranchPassDifferentPage) {
    u16 start = 0x10F0;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BCC_BranchBackwards) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x80;  // -128 == -0x80
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.PC == start - 128);
}
// Other branching instructions just single branch
TEST_F(BRANCH, BCS_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCS_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BEQ_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BEQ_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_z(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BMI_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BMI_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_n(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BNE_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BNE_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_z(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BPL_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BPL_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_n(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BVC_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BVC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_v(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BVS_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BVS_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_v(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}


// Subroutine test
TEST_F(SUBROUTINE, SubroutineJumpToAndReturnFrom) {
    cpu.PC = 0x4000;
    // Jump to subroutine
    cpu[0x4000] = JSR_ABS;
    cpu[0x4001] = 0x34;
    cpu[0x4002] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.PC == 0x1234);
    // subroutine doesn't do anything except return
    cpu[0x1234] = NOP_IMP;
    cpu[0x1235] = NOP_IMP;
    cpu[0x1236] = RTS_IMP;
    ASSERT_TRUE(cpu.execute(10) == 10);
    ASSERT_TRUE(cpu.PC == 0x4003);
}
