HDR := $(wildcard *.h)
SRC := $(shell find . -maxdepth 1 -name '[a-z]*\.c' -printf '%P\n')
OBJ := $(SRC:%.c=tmp/%.o)
CC = gcc

run: tmp bin/Test bin/Hive
	rm -rf core.*
	stdbuf --output=L bin/Test && stdbuf --output=L bin/Hive

bin/%: tmp/%.o ${OBJ} 
	${CC} -g -O0 $^ -o $@

tmp/%.o: %.c 
	$(CC) $(CFLAGS) -g -c $< -o $@

tmp:
	mkdir -p tmp bin

.PHONY: clean run tmp

clean:
	rm -rf bin tmp Test Hive .depend tags *.pile

${OBJ}: Makefile $(HDR)

depend: .depend

.depend: $(SRC)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ > "$@"

include .depend

tags: $(SRC) $(HDR)
	ctags *.h *.c test/*.c

.SUFFIXES:

