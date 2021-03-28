#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// Push stack
TEST_F(PUSH, PHA) {
    cpu[RESET_START] = PHA_IMP;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.S == 0xfe);
    ASSERT_TRUE(cpu[0x01ff] == 0x42);
}
TEST_F(PUSH, PHP) {
    cpu[RESET_START] = PHP_IMP;
    cpu.SR = 0b10101010;    // Arbitrary flags
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.S == 0xfe);
    ASSERT_TRUE(cpu[0x01ff] == 0b10101010);
}

// Pull stack
TEST_F(PULL, PLA) {
    // Zero
    cpu[RESET_START] = PLA_IMP;
    cpu.A = 0x11;
    cpu[0x01ff] = 0x0; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xff);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = PLA_IMP;
    cpu.A = 0;
    cpu[0x01ff] = 0x42; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xff);
    ASSERT_TRUE(cpu.A == 0x42);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = PLA_IMP;
    cpu.A = 0;
    cpu[0x01ff] = 0xFF; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xFF);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(PULL, PLP) {
    cpu[RESET_START] = PLP_IMP;
    cpu.SR = 0;
    cpu[0x01ff] = 0x42; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xff);
    ASSERT_TRUE(cpu.SR == 0x42);
}
