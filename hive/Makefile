HDR := $(wildcard *.h)
SRC := $(wildcard *.c)
OBJ := $(SRC:%.c=tmp/%.o)
TARGET := hive 
CC = gcc

run: ${TARGET}
	./${TARGET}

${TARGET}: ${OBJ} 
	${CC} -g -O0 $^ -o $@

tmp/%.o: %.c 
	$(CC) $(CFLAGS) -g -c $< -o $@

tmp: tags
	mkdir -p tmp

.PHONY: clean run

clean:
	rm -f ${OBJ} ${TARGET}

${OBJ}: tmp Makefile

depend: .depend

.depend: $(SRC)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ > "$@"

include .depend

tags: $(SRC) $(HDR)
	ctags *.h *.c

