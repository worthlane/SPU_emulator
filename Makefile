all: asm disasm spu

asm:
	cd asm && make makedirs && make && cd ..

spu:
	cd SPU && make makedirs && make && cd ..

.PHONY: asm spu

.PHONY: clean disasm

clean:
	cd asm && make clean && cd ..
	cd disasm && make clean && cd ..
	cd SPU && make clean && cd ..
	cd common && rm -rf *.o && cd ..

disasm:
	cd disasm && make makedirs && make && cd ..
