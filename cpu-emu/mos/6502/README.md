# MOS 6502

## Links

* http://www.6502.org/
* ISA: https://www.masswerk.at/6502/6502_instruction_set.html

## Overview
* 8 bit microcontroller
* 16 bit data bus (ie 2^16 = 64 KiB max memory)
* "zero page" the first 256 bytes of memory indended to be used as registers, special addressing mode to access them easily

* Processor Stack: LIFO, top down, 8 bit range, 0x0100 - 0x01FF

* Signed values are two's complement, sign in bit 7 (most significant bit).
	* (%11111111 = $FF = -1, %10000000 = $80 = -128, %01111111 = $7F = +127)
	* good, assuming modern host CPU does two's complement arithmetic.

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
* 3 - D - decimal
* 4 - B - break
* 5 -     ignored
* 6 - V - overflow
* 7 - N - negative

## Addressing Modes

Accumulator, Immediate, Absolute, Zero-page, Indexed Zero-page, Indexed absolute, Implied, relative, indexed indirect, indirect indexed, absolute indirect

## Instructions
```sh
# Arithmetic
ADC	....	add with carry
AND	....	and (with accumulator)
ASL	....	arithmetic shift left
DEC	....	decrement
DEX	....	decrement X
DEY	....	decrement Y
INC	....	increment
INX	....	increment X
INY	....	increment Y
SBC	....	subtract with carry

# Bitwise arithmetic
EOR	....	exclusive or (with accumulator)
ORA	....	or with accumulator
ROL	....	rotate left
ROR	....	rotate right

# Shifting
LSR	....	logical shift right

# Comparison 
BIT	....	bit test
CMP	....	compare (with accumulator)
CPX	....	compare with X
CPY	....	compare with Y

# Branch
BCC	....	branch on carry clear
BCS	....	branch on carry set
BEQ	....	branch on equal (zero set)
BMI	....	branch on minus (negative set)
BNE	....	branch on not equal (zero clear)
BPL	....	branch on plus (negative clear)
BVC	....	branch on overflow clear
BVS	....	branch on overflow set
JMP	....	jump
JSR	....	jump subroutine
RTS	....	return from subroutine

# Clear flag bits
CLC	....	clear carry
CLD	....	clear decimal
CLI	....	clear interrupt disable
CLV	....	clear overflow

# Load / Store
LDA	....	load accumulator
LDX	....	load X
LDY	....	load Y
SEC	....	set carry
SED	....	set decimal
SEI	....	set interrupt disable
STA	....	store accumulator
STX	....	store X
STY	....	store Y
TAX	....	transfer accumulator to X
TAY	....	transfer accumulator to Y
TSX	....	transfer stack pointer to X
TXA	....	transfer X to accumulator
TXS	....	transfer X to stack pointer
TYA	....	transfer Y to accumulator

# Push / pop stack
PHA	....	push accumulator
PHP	....	push processor status (SR)
PLA	....	pull accumulator
PLP	....	pull processor status (SR)

# Interrupts
BRK	....	break / interrupt
RTI	....	return from interrupt

NOP	....	no operation
```

## Assembler syntax

...