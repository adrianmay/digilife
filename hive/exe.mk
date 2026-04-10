CC = gcc
TMPDIR = ../tmp
TGTDIR = ../bin
OBJDIR = ${TMPDIR}/${NAME}
TGT = ${TGTDIR}/${NAME}
HDR = $(wildcard ../h/*.h) $(wildcard *.h)
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=${OBJDIR}/%.o)
DEP = $(OBJ:.o=.d)

CFLAGS = -g -iquote ../h -MMD -MP

run: tags ${TGT}
	rm -f core.*
	stdbuf --output=L ${TGT}

${TGT}: ${OBJ} ../tmp/lib/hive.a
	@mkdir -p ${TGTDIR}
	${CC} -g -O0 $^ -o $@

${OBJDIR}/%.o: %.c 
	@mkdir -p ${OBJDIR}
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)	

../tmp/lib/hive.a: ../h ../lib
	${MAKE} -C ../lib

C := $(shell find .. -name '[a-z]*\.c' )
H := $(shell find .. -name '[a-z]*\.h' )

tags: ${H} ${C}
	ctags ${H} ${C} 

clean: 
	rm -rf ${OBJDIR} ${TGT}

