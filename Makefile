all: asm disasm spu clean

asm:
	cd asm && make makedirs && make && cd ..

disasm:
	cd disasm && make makedirs && make && cd ..

spu:
	cd SPU && make makedirs && make && cd ..

clean:
	cd asm && make clean && cd ..
	cd disasm && make clean && cd ..
	cd SPU && make clean && cd ..

.PHONY: asm disasm spu clean
