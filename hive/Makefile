HDR := $(wildcard *.h)
SRC := $(shell find . -name '[a-z]*\.c')
OBJ := $(SRC:%.c=tmp/%.o)
CC = gcc

run: Test Hive
	./Test && ./Hive

%: tmp/%.o ${OBJ} 
	${CC} -g -O0 $^ -o $@

tmp/%.o: %.c 
	$(CC) $(CFLAGS) -g -c $< -o $@

tmp: tags
	mkdir -p tmp

.PHONY: clean run

clean:
	rm -rf tmp Test Hive .depend tags

${OBJ}: tmp Makefile

depend: .depend

.depend: $(SRC)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ > "$@"

include .depend

tags: $(SRC) $(HDR)
	ctags *.h *.c

.SUFFIXES:

