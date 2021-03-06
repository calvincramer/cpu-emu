# MOS 6502

## Build
```sh
# Optimized
cd <repo>/cpu-emu/mos/6502
mkdir build-optim
cd build-optim
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Debug
cd <repo>/cpu-emu/mos/6502
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

## Links

* Main: http://www.obelisk.me.uk/6502/
* http://www.6502.org/
* https://www.masswerk.at/6502/6502_instruction_set.html
* http://visual6502.org/
* Commodore 64 memory map: https://sta.c64.org/cbm64mem.html

## Overview
* 8 bit microcontroller
* 16 bit data bus (ie 2^16 = 64 KiB max memory)
* "zero page" the first 256 bytes of memory intended to be used as registers, special addressing mode to access them easily

* Processor Stack: grows down, 8 bit range, 0x01FF to 0x0100

* Signed values are two's complement, sign in bit 7 (most significant bit).
    * (%11111111 = $FF = -1, %10000000 = $80 = -128, %01111111 = $7F = +127)
    * good, assuming modern host CPU does two's complement arithmetic.

* There are three 2-byte address locations at the very top end of the 64K address space serving as jump vectors for reset/startup and interrupt operations:

$FFFA, $FFFB ... NMI (Non-Maskable Interrupt) vector
$FFFC, $FFFD ... RES (Reset) vector
$FFFE, $FFFF ... IRQ (Interrupt Request) vector

* At the occurrence of interrupt, the value of the program counter (PC) is put in high-low order onto the stack, followed by the value currently in the status register and control will be transferred to the address location found in the respective interrupt vector. These are recovered from the stack at the end of an interrupt routine by the RTI instruction.

* since zero page is 256 bytes, this implies each page is 256 bytes

* instructions are variable width.

* "Very conveniently, the 6502 can add and subtract unsigned numbers using the ADC and SBC instructions." http://www.6502.org/tutorials/vflag.html


## Memory layout

0000 - 00FF     zero page
0100 - 01FF     stack
0200 - FFFF     general purpose


## Registers

* A - accumulator 8 bits
* X, Y - index registers, 8 bits each
* P - 6 flags, 1-bit each:
* S - stack pointer 8 bits
* PC - program counter 16 bits

## Flags

* 0 - C - carry
* 1 - Z - zero
* 2 - I - interrupt (irq disable)
* 3 - D - decimal (binary coded decimal mode, only affects ADC and SBC instructions)
* 4 - B - break
* 5 -     ignored
* 6 - V - overflow
* 7 - N - negative

## Addressing Modes

The addressing mode determines the operand of the instruction

(IMP) Accumulator       A       A reg is operand (basically implied)

(IMP) Implied           i       implied (TXA)

(IMM) Immediate         #       A reg (LDA)


(ZPG) Zero-page         zp      address is in zero page (LDY $02)

(ZPX) Zero-page, x

(ZPY) Zerp-page, y


(ABS) Absolute          a       (LDX) isn't this also implied?

(ABX) Absolute, x       a,X     address at absolute address plus X used for operation

(ABY) Absolute, y       a,y

(IND) Absolute indirect (a)     used by JMP to get 16 bit value -> JMP ($A001) jumps to value at $A001 + ($A002 << 8)
                                Weird behavior:
                                    0x3000: 0x40
                                    0x30FF: 0x80
                                    0x3100: 0x50
                                    JMP ($30FF) jumps to 0x4080 rather than expected 0x5080.
                                        Low byte is correct
                                        High byte wraps back to 0x3000 rather than increase to 0x3100

(IDX) (Indirect, x)     (aka indexed indirect)
                        operand is zero-page address
                        gets 16 bit memory address from 2-byte value starting at address (operand + X)

(IDY) (Indirect), y     (aka indirect indexed)
                        operand is zero-page address
                        gets 16 bit memory adderss from Y + (2-byte value starting at operand)


(REL) relative          r       PC + offset (BPL $2D)




Notes:
* Zero-page, x will wrap around 8 bit value (LDA $FF,X when X is 1 will load $0 into A)




## Instructions
```sh
# Clear flag bits
CLC     clear carry
CLD     clear decimal
CLI     clear interrupt disable
CLV     clear overflow

# Set flags
SEC     set carry
SED     set decimal
SEI     set interrupt disable

# Load / Store
LDA     load accumulator
LDX     load X
LDY     load Y
STA     store accumulator
STX     store X
STY     store Y

# Transfer between registers
TAX     transfer accumulator to X
TAY     transfer accumulator to Y
TSX     transfer stack pointer to X
TXA     transfer X to accumulator
TXS     transfer X to stack pointer
TYA     transfer Y to accumulator

# Arithmetic
ADC     add with carry
SBC     subtract with carry
INC     increment
INX     increment X
INY     increment Y
DEC     decrement
DEX     decrement X
DEY     decrement Y
AND     and (with accumulator)
EOR     exclusive or (with accumulator)
ORA     or (with accumulator)
ASL     arithmetic shift left
LSR     logical shift right
ROL     rotate left
ROR     rotate right

# Comparison
BIT     bit test
CMP     compare (with accumulator)
CPX     compare with X
CPY     compare with Y

# Push / pop stack
PHA     push accumulator
PHP     push processor status (SR)
PLA     pull accumulator
PLP     pull processor status (SR)

# Branching
BCC     branch on carry clear
BCS     branch on carry set
BEQ     branch on equal (zero set)
BMI     branch on minus (negative set)
BNE     branch on not equal (zero clear)
BPL     branch on plus (negative clear)
BVC     branch on overflow clear
BVS     branch on overflow set
JMP     jump
JSR     jump subroutine
RTS     return from subroutine

# Interrupts
BRK     break / interrupt
RTI     return from interrupt

# etc
NOP     no operation
```

## Assembler syntax

* Varies between assemblers
```c
// Binary #%
%00001111       LDA #%0001
// Hexadecimal #$
$FA             LDA #$0E
// Decimal #
123             LDA #123
```


## Etc

* instructions a 8 bits
* bits 7-5 are 'a', 4-2 are 'b', 1-0 are 'c' (aaabbbcc)
* BCD invalid numbers are undocumented behavior