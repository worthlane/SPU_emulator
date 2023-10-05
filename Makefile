all: asm spu disasm

asm:
    cd asm && makedirs && make && cd ..

spu:
    cd SPU && makedirs && make && cd ..

disasm:
    cd disasm && makedirs && make && cd ..
