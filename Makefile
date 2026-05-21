a.elf: main.c
	gcc -g -O3 main.c -o a.elf -lm -lglfw -lGL

.PHONY = ct c

ct: a.elf
	./a.elf
	rm a.elf
c:
	rm a.elf
