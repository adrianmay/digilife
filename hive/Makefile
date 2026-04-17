CC = gcc
CFLAGS = -g -iquote h -MMD -MP -Wall -Werror

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

ALLSRCSRC = $(shell find lit/* tpt/* -name '*.c' -or -name '*.h')
ALLSRC    = $(ALLSRCSRC:%=gen/%)
 
tags: ${ALLSRCSRC}
	echo "ALLSRC=" $(ALLSRC)
	ctags ${ALLSRCSRC}

clean:
	rm -rf gen obj exe .depend tags *.pile

.depend: $(ALL)
	find gen -name "*.c" - | while read C; do grep -Po '^#include "\K[^"]*' $C | sed "s#^#$C: #"; done > .depend


