#
#
#
.SUFFIXES:

.SUFFIXES:	.c .o

.c.o:
	gcc -c -Wall -O2 -o $*.o $*.c
#
#
files	= main.o asm11.o hash.o debug.o opcode.o
#
#
#
#
asm11 : $(files)
	gcc -O2 -s $(files) -o $@
#	cp asm16.exe cina16
#
test:
	./asm11 -l div.asm
	../r11sim/r11 div.bin

#	cat infile.lst
#
test1:
	./asm11 -l infile.asm
	cp  infile.bin ../r11sim/testdata.bin
#
#
clean:
	-rm *.o
	-rm asm11.exe
	-rm main
#
#
