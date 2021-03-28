#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Set flags
TEST_F(SET_FLAGS, SetDifferentFlags) {
    // Carry
    cpu[RESET_START] = SEC_IMP;
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == 1);

    // Decimal
    cpu.reset();
    cpu[RESET_START] = SED_IMP;
    cpu.set_flag_d(0);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_d() == 1);

    // Interrupt Disable
    cpu.reset();
    cpu[RESET_START] = SEI_IMP;
    cpu.set_flag_i(0);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_i() == 1);
}
