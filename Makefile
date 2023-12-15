all: asm spu

asm:
	cd asm && make makedirs && make && cd ..

spu:
	cd spu && make makedirs && make && cd ..

.PHONY: asm spu

.PHONY: clean disasm run

run:
	./asm/asm assets/asm_code.txt assets/byte_code.txt assets/byte_code.bin && ./SPU/spu assets/byte_code.bin

clean:
	cd asm && make clean && cd ..
	cd disasm && make clean && cd ..
	cd spu && make clean && cd ..
	cd common && rm -rf *.o && cd ..

disasm:
	cd disasm && make makedirs && make && cd ..

# -I
