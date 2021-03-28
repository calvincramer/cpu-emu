#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Interrupt
TEST_F(INTERRUPT, BRK_and_RTI) {
    // Set interrupt handler location
    cpu[INT_VEC_LOC] = 0x34;
    cpu[INT_VEC_LOC + 1] = 0x12;

    // Generate interrupt
    cpu[RESET_START] = BRK_IMP;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu.PC == 0x1234);

    // Do something in interrupt and return
    cpu[0x1234] = NOP_IMP;
    cpu[0x1235] = NOP_IMP;
    ASSERT_TRUE(cpu.execute(4) == 4);
    cpu[0x1236] = RTI_IMP;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.PC == RESET_START);  // Should return to original location
}
