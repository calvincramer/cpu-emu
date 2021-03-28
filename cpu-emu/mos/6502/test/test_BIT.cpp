#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// BIT
TEST_F(BIT, ZeroPage) {
    // Zero
    cpu.reset();
    cpu[RESET_START] = BIT_ZPG;
    cpu[RESET_START + 1] = 0x15;
    cpu[0x15] = 0xF3;   // 0b 1111 0011
    cpu.A = 0x0C;       // 0b 0000 1100
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_v() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Not zero
    cpu.reset();
    cpu[RESET_START] = BIT_ZPG;
    cpu[RESET_START + 1] = 0x15;
    cpu[0x15] = 0xF3;   // 0b 1111 0011
    cpu.A = 0x99;       // 0b 1001 1001
    ASSERT_TRUE(cpu.execute(3) == 3);   // Anded value should be 0b 1001 0001
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_v() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(BIT, Absolute) {
    cpu.reset();
    cpu[RESET_START] = BIT_ABS;
    cpu[RESET_START + 1] = 0x15;
    cpu[RESET_START + 2] = 0x14;
    cpu[0x1415] = 0xF3;     // 0b 1111 0011
    cpu.A = 0x99;           // 0b 1001 1001
    ASSERT_TRUE(cpu.execute(4) == 4);   // Anded value should be 0b 1001 0001
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_v() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
