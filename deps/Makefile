FILES	:= testelf1 testelf2

CC 	:= gcc
FLAGS 	:= -nostdlib -static -pie -fno-pic -fpie -z max-page-size=0x1000 -fno-asynchronous-unwind-tables
SOURCE	:= testelf.c

.PHONY: all

all: $(FILES)

# please PR a better solution, it's 5 AM here

testelf1:
	$(CC) $(FLAGS) -o $@ testelf1.c

testelf2:
	$(CC) $(FLAGS) -o $@ testelf2.c

clean:
	rm $(FILES)
