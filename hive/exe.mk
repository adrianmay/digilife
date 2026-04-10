CC = gcc
TMPDIR = ../tmp
TGTDIR = ../bin
OBJDIR = ${TMPDIR}/${NAME}
TGT = ${TGTDIR}/${NAME}
HDR = $(wildcard ../h/*.h)
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=${OBJDIR}/%.o)
DEP = $(OBJ:.o=.d)

CFLAGS = -g -iquote ../h -MMD -MP

run: ${TGT}
	rm -rf core.*
	stdbuf --output=L ${TGT}

${TGT}: ${OBJ} ../tmp/lib/hive.a
	@mkdir -p ${TGTDIR}
	${CC} -g -O0 $^ -o $@

${OBJDIR}/%.o: %.c 
	@mkdir -p ${OBJDIR}
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)	

../tmp/lib/hive.a: 
	${MAKE} -C ../lib

clean: 
	rm -rf ${OBJDIR} ${TGT}

