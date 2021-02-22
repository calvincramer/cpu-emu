#include <gtest/gtest.h>
#include "mos6502.hpp"

using namespace mos6502;

TEST(SimpleTests, ApiTestReset) {
	CPU cpu;
	cpu.reset();
}

TEST(SimpleTests, ApiTestExecute) {
	CPU cpu;
	cpu.execute(0);
}


int main(int argc, char **argv) 
    {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    }
