.PHONY = ct c

ctdbg: main.c
	gcc -DDBG_CULLING_MODE=1 -O3 -funroll-loops -march=native main.c -o a.elf -lm -lglfw -lGL
	./a.elf
	rm a.elf
ctrel: main.c
	gcc -DDBG_CULLING_MODE=0 -O3 -funroll-loops -march=native main.c -o a.elf -lm -lglfw -lGL
	./a.elf
	rm a.elf
