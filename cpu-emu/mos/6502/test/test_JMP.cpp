#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// JMP
TEST_F(JMP, Absolute) {
    cpu[RESET_START] = JMP_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == 0x1234);
}
TEST_F(JMP, Indirect) {
    cpu[RESET_START] = JMP_IND;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x04;
    cpu[0x1235] = 0x40;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.PC == 0x4004);
}
TEST_F(JMP, IndirectBoundary) {
    cpu[RESET_START] = JMP_IND;
    cpu[RESET_START + 1] = 0xFF;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1200] = 0xEE;
    cpu[0x12FF] = 0x04;
    cpu[0x1300] = 0x40;
    ASSERT_TRUE(cpu.execute(5) == 5);
#if CPU_MODEL == MODEL_6502
    ASSERT_TRUE(cpu.PC == 0xEE04);
#else
    ASSERT_TRUE(cpu.PC == 0x4004);
#endif
}