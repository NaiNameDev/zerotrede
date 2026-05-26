BM = 0

CC = gcc
DFLGS = -DDBG_CULLING_MODE=1 -DBARI_MODE=$(BM) -std=gnu23 -O3 -Wall -Wextra
RFLGS = -DDBG_CULLING_MODE=0 -DBARI_MODE=$(BM) -std=gnu23 -Ofast -Wall -Wextra -fwhole-program -flto -freciprocal-math -fno-math-errno -ffinite-math-only -funsafe-math-optimizations -ffast-math -funroll-loops -march=native
LIBS = -lm -lglfw -lGL

.PHONY = ct c

ctdbg: main.c
	$(CC) $(DFLGS) main.c -o a.elf $(LIBS)
	./a.elf
	rm a.elf
ctrel: main.c
	$(CC) $(RFLGS) main.c -o a.elf $(LIBS)
	./a.elf
	rm a.elf
