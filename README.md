# SPU_emulator
Software processing unit, built on mine stack realization https://github.com/worthlane/cursed_stack
## Installation
```
make
```
## Assembler
Contains in "asm" folder, translates assembler code to byte code and prints it in two different formats: TXT and BIN. SPU uses binary file, while txt needed for debug.

## Disassembler
Contains in "disasm" folder. Translates byte code to assembler code. Reads information from binary input file.

## Commands
1) push [value] - to push element in stack
2) pop [target] - to pop element from stack and give it's value for register
3) out - to print last element from stack (it pops from stack)
4) sub - to subtract the last one from the penultimate element
5) add - to add two last elements
6) div - to divide the penultimate element by the last
7) mul - to multiply two last stack elements
8) sqrt - the square root of the last stack element
9) cos - the cosinus of the last stack element
10) sin - the sinus of the last stack element
11) in - push value in stack from keyboard input
12) hlt - to stop processor

## Registers
SPU has 4 registers:
1) rax
2) rbx
3) rcx
4) rdx  

Example:
```
push 100
push 300
pop rcx
```

rcx = 300.

## Default configuration

Default files:  
Assmebler code: "assets/asm_code.txt"  
Byte code in txt: "assets/byte_code.txt"  
Byte code in binary: "assets/byte_code.bin"
