FILES	:= testelf1 testelf2

CC 	:= gcc
FLAGS 	:= -nostdlib -static -pie -fno-pic -fpie -O0 -z max-page-size=0x1000 \
	   -fno-asynchronous-unwind-tables -fno-stack-protector
SOURCE	:= testelf.c

.PHONY: all

all: $(FILES)

# please PR a better solution, it's 5 AM here

testelf1: testelf1.c
	$(CC) $(FLAGS) -o $@ testelf1.c

testelf2: testelf2.c
	$(CC) $(FLAGS) -o $@ testelf2.c

clean:
	rm $(FILES)
