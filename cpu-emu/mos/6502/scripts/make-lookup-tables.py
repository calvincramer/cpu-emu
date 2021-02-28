#!/usr/bin/python3

import os

split_on = 16

fileName = "opcodes-bytes-cycles.txt"
contents = open(fileName ,'r').read()

lines = contents.split('\n')
lines = lines[1:]	# Header

for i in range(len(lines)):
	lines[i] = lines[i].split('\t')
	lines[i][0] = int(lines[i][0][1:].lower(), 16)

num_cycles = [0] * 256
instr_bytes = [0] * 256

for opcode, _bytes, cycles in lines:
	num_cycles[opcode] = cycles
	instr_bytes[opcode] = _bytes

print("Num cycles base lkup:")

for i in range(len(num_cycles)):
	if i % split_on == 0:
		print()
	print(f'{num_cycles[i]}, ', end='')

print("\n\nInstruction bytes lkup:")

for i in range(len(instr_bytes)):
	if i % split_on == 0:
		print()
	print(f'{instr_bytes[i]}, ', end='')

print()
