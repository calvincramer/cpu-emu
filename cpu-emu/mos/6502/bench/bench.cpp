#include <chrono>

#include <cstdio>
#include <ctime>

#include "mos6502.hpp"
#include "models.hpp"

using namespace mos6502;

void setupCPU(CPU& cpu) {
    cpu[RESET_START + 0] = ADC_IMM;
    cpu[RESET_START + 1] = 1;
    cpu[RESET_START + 2] = ADC_IMM;
    cpu[RESET_START + 3] = 1;
    cpu[RESET_START + 4] = ADC_IMM;
    cpu[RESET_START + 5] = 1;
    cpu[RESET_START + 6] = ADC_IMM;
    cpu[RESET_START + 7] = 1;
    cpu[RESET_START + 8] = ADC_IMM;
    cpu[RESET_START + 9] = 1;
    cpu[RESET_START + 10] = JMP_ABS;
    cpu[RESET_START + 11] = lowByte(RESET_START);
    cpu[RESET_START + 12] = lowByte(RESET_START);
}

int main() {
    CPU cpu;
    long maxCycles[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000};

    printf("%16s %16s %16s\n", "Cycles", "Time(s)", "Speed(Mcylces/s)");

    for (long max_c : maxCycles) {
        cpu.reset();
        setupCPU(cpu);

        auto start = std::chrono::system_clock::now();
        cpu.execute(max_c);
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;
        printf("%16ld %16f %16f\n", max_c, elapsed_seconds.count(), (max_c / elapsed_seconds.count()) / 1000000);
    }
    return 0;
}
