#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;


// No op
TEST_F(NOP, MultipleNoOp) {
    cpu[RESET_START] = NOP_IMP;
    cpu[RESET_START + 1] = NOP_IMP;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.PC == RESET_START + 2);
}
