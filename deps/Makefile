CC 	:= gcc
FLAGS 	:= -nostdlib -static -pie -fno-pic -fpie -z max-page-size=0x1000 -fno-asynchronous-unwind-tables
SOURCE	:= testelf.c

.PHONY: all

all: testelf

testelf: testelf.c
	$(CC) $(FLAGS) -o $@ $<

clean:
	rm testelf
