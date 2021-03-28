#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Clear flags
TEST_F(CLEAR_FLAGS, ClearDifferentFlags) {
    // Carry
    cpu[RESET_START] = CLC_IMP;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == 0);

    // Decimal
    cpu.reset();
    cpu[RESET_START] = CLD_IMP;
    cpu.set_flag_d(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_d() == 0);

    // Interrupt Disable
    cpu.reset();
    cpu[RESET_START] = CLI_IMP;
    cpu.set_flag_i(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_i() == 0);

    // Overflow
    cpu.reset();
    cpu[RESET_START] = CLV_IMP;
    cpu.set_flag_v(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_v() == 0);
}
