CC = gcc
CFLAGS = -g -iquote h -MMD -MP -Wsign-conversion -Werror

BUILDDIR = .
OBJDIR   = ${BUILDDIR}/obj
OUTDIR   = ${BUILDDIR}/out

${OBJDIR}/%.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

lib      = ${OUTDIR}/hive.a
test     = ${OUTDIR}/test
hive     = ${OUTDIR}/hive

run: ${test} ${hive} tags
	rm -f core.*
	rm -f *.pile
	stdbuf --output=L ${test} && stdbuf --output=L ${hive}

define make-obj
$(1)C   := $$(wildcard $(1)/*.c)
$(1)OBJ := $$($(1)C:%.c=$${OBJDIR}/%.o)
$(1)DEP := $$($(1)OBJ:.o=.d)
-include $$($(1)DEP)
endef

define make-lib
$(call make-obj,$(1))
$$($(1)): $$($(1)OBJ)
	@mkdir -p ${OUTDIR}
	ar rcs $$@ $$^
endef

define make-program
$(call make-obj,$(1))
$$($(1)): $$($(1)OBJ) $$(lib)
	$$(CC) -g -O0 $$^ -o $$@
endef

$(eval $(call make-lib,lib))
$(eval $(call make-program,hive))
$(eval $(call make-program,test))

ALL = $(shell find . -name '*.c' -or -name '*.h')
 
tags: ${ALL}
	ctags ${ALL}

clean:
	rm -rf ${OUTDIR} ${OBJDIR} *.pile


