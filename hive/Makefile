CC = gcc
CFLAGS = -g -iquote h -MMD -MP

BUILDDIR = .
OBJDIR   = ${BUILDDIR}/obj
OUTDIR   = ${BUILDDIR}

LIB      = ${OBJDIR}/hive.a
TEST     = ${OUTDIR}/Test
HIVE     = ${OUTDIR}/Hive

run: ${TEST} ${HIVE}
	rm -f core.*
	rm -f *.pile
	stdbuf --output=L ${TEST} && stdbuf --output=L ${HIVE}

#################################

HIVEOBJDIR = ${OBJDIR}/hive
HIVEH   = $(wildcard hive/*.h)
HIVEC   = $(wildcard hive/*.c)
HIVEOBJ = $(HIVEC:%.c=${OBJDIR}/%.o)
HIVEDEP = $(HIVEOBJ:.o=.d)

${HIVE}: ${HIVEOBJ} ${LIB}
	@mkdir -p ${OUTDIR}
	${CC} -g -O0 $^ -o $@

${HIVEOBJDIR}/%.o: hive/%.c 
	@mkdir -p ${HIVEOBJDIR}
	$(CC) $(CFLAGS) -c $< -o $@


#################################

TESTOBJDIR = ${OBJDIR}/test
TESTH   = $(wildcard test/*.h)
TESTC   = $(wildcard test/*.c)
TESTOBJ = $(TESTC:%.c=${OBJDIR}/%.o)
TESTDEP = $(TESTOBJ:.o=.d)

${TEST}: ${TESTOBJ} ${LIB}
	@mkdir -p ${OUTDIR}
	${CC} -g -O0 $^ -o $@

${TESTOBJDIR}/%.o: test/%.c 
	@mkdir -p ${TESTOBJDIR}
	$(CC) $(CFLAGS) -c $< -o $@

#################################

LIBOBJDIR = ${OBJDIR}/lib
LIBH = $(wildcard h/*.h)
LIBC = $(wildcard lib/*.c)
LIBOBJ = $(LIBC:%.c=${OBJDIR}/%.o)
LIBDEP = $(LIBOBJ:.o=.d)

${LIB}: ${LIBOBJ}
	@mkdir -p ${OUTDIR}
	ar rcs $@ $^

${LIBOBJDIR}/%.o: lib/%.c 
	@mkdir -p ${LIBOBJDIR}
	$(CC) $(CFLAGS) -c $< -o $@

#################################

ALL = ${LIBH} ${LIBC} ${TESTH} ${TESTC} ${HIVEH} ${HIVEC}
 
tags: ${ALL}
	ctags ${ALL}

-include $(LIBDEP)  $(TESTDEP)	$(HIVEDEP)	

clean:
	rm -rf ${TEST} ${HIVE} ${OBJDIR}




