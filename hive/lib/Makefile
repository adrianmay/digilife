CC = gcc
OUT = ../tmp/lib
HDR = $(wildcard ../h/*.h)
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=${OUT}/%.o)
DEP = $(OBJ:.o=.d)
LIB = ${OUT}/hive.a

CFLAGS = -g -iquote ../h -MMD -MP

${LIB}: ${OBJ}
	ar rcs $@ $^

${OUT}/%.o: %.c 
	@mkdir -p ${OUT}
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)	

clean: 
	rm -rf ${OUT}
