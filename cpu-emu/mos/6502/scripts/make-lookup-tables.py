#!/usr/bin/python3

import os

split_on = 16

# fileName = "opcodes-bytes-cycles.txt"
fileName = "address-mode-opcodes-bytes-cycles.txt"
contents = open(fileName ,'r').read()

lines = contents.split('\n')
lines = lines[1:]   # Take out first line header

# Preprocess
for i in range(len(lines)):
    lines[i] = lines[i].split('\t')
    lines[i][1] = int(lines[i][1][1:].lower(), 16)  # Opcode (like) $3D to int

# Tables
address_modes = ['NUL'] * 256
num_cycles = [0] * 256
instr_bytes = [0] * 256

# Put info in tables per instruction
for addr_mode, opcode, _bytes, cycles in lines:
    address_modes[opcode] = addr_mode
    num_cycles[opcode] = cycles
    instr_bytes[opcode] = _bytes

def print_lkup_table(table):
    for i in range(len(table)):
        if i % split_on == 0:
            print()
        print(f'{table[i]}, ', end='')
    print()
    print()

print("Address modes lkup:")
print_lkup_table(address_modes)

print("Num cycles base lkup:")
print_lkup_table(num_cycles)

print("Instruction bytes lkup:")
print_lkup_table(instr_bytes)
