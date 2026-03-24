HDR := $(wildcard *.h)
SRC := $(wildcard *.c)
OBJ := $(SRC:%.c=tmp/%.o)
CC = gcc

run: test hive
	./test && ./hive

%: ${OBJ} tmp/%.o
	${CC} -g -O0 $^ -o $@

tmp/%.o: %.C 
	$(CC) $(CFLAGS) -g -c $< -o $@

tmp/%.o: %.c 
	$(CC) $(CFLAGS) -g -c $< -o $@

tmp: tags
	mkdir -p tmp

.PHONY: clean run

clean:
	rm -f ${OBJ} test hive

${OBJ}: tmp Makefile

depend: .depend

.depend: $(SRC)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ > "$@"

include .depend

tags: $(SRC) $(HDR)
	ctags *.h *.c

