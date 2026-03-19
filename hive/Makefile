SRC := $(wildcard *.c)
OBJ := $(SRC:%.c=tmp/%.o)
TARGET := hive
CC = gcc

run: ${TARGET}
	./${TARGET}

${TARGET}: ${OBJ}
	 ${CC} $^ -o $@

tmp/%.o: %.c | tmp
	$(CC) $(CFLAGS) -c $< -o $@

# ${OBJ}: ${SRC} | tmp
# 	 ${CC} -c $^ -o $@

tmp:
	mkdir -p tmp

.PHONY: clean run

clean:
	rm -f ${OBJ} ${TARGET}

