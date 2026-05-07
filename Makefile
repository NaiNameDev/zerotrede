a.elf: main.c
	gcc -O3 main.c -o a.elf -lm -lSDL2

.PHONY = ct c

ct: a.elf
	./a.elf
	rm a.elf
c:
	rm a.elf
