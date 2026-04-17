CFLAGS = -g -iquote gen/h -Wall -Werror
CC = gcc $(CFLAGS)

###########################################################
# Copy lits to gen verbatim
#
#


lits   = $(shell find lit -type f)
dits   = $(lits:lit/%=gen/%)

gen/%: lit/%
	mkdir -p $(dir $@)
	cp $^ $@

all: tags
	echo "Doing all"

# all: $(gits)
# 	echo $(gits)

###########################################################
# Build any .c under gen to the corresponding .o under obj

define make-obj

$(1)LITC = $$(wildcard lit/$(1)/*.c)
$(1)LITH = $$(wildcard lit/$(1)/*.h)
$(1)C    = $$($(1)LITC:lit/%=gen/%)
$(1)H    = $$($(1)LITH:lit/%=gen/%)
$(1)OBJ  = $$($(1)C:gen/%.c=obj/%.o)
$(1)HHD  = $(2) # Dunno why I need this step
$(1)HDEP = $$($(1)H) $$($(1)HHD:%=gen/h/%.h)

obj/$(1)/%.o: gen/$(1)/%.c | $$($(1)HDEP)
	echo $(1) $$($(1)HDEP)
	mkdir -p $$(dir $$@)
	$$(CC) -c $$< -o $$@

obj/$(1).o: $$($(1)OBJ)
	echo $$($(1)HDEP)
	mkdir -p obj
	ld --relocatable -o $$@ $$^

gen/h/$(1).h: gen/$(1)/$(1).h # Export the main header
	mkdir -p gen/h
	ln -fs ../../$$^ $$@

# obj/$(2)_$(1).o: gen/h/$(2)_$(1).h # Provide a publicly visible version of the obj

all: obj/$(1).o

endef

###########################################################

define make-exe


exe/$(1): obj/$(1).o obj/$(2).o | gen/h/$(2).h
	mkdir -p exe
	$$(CC) $$^ -o $$@

all: exe/$(1)


endef

###########################################################
# Params are template name, generated module name, replacements
define use-template

$(2)ts = $$(wildcard tpt/$(1)/*) # All the files in the template - should be a find
$(2)gs = $$($(2)ts:tpt/$(1)/%=gen/$(2)/%) # Corresponding paths in the module

gen/$(2)/%: tpt/$(1)/%  # Copy and apply replacements
	mkdir -p gen/$(2)
	awk $(3) '{ gsub(/XX/,X); gsub(/YY/,Y); }1' $$^ > $$@

gen/$(2)/%: Makefile # Dunno if this is working

# all: $$($(2)gs) gen/h/$(2)_$(1).h # Kick make into requiring the gen file. Should be obsolete if the gen file is required by something else

# all: obj/$(2)_$(1).o # Kick make into making the public obj

endef
###########################################################
# $(eval $(call use-template,pile,foo,-v X=xxxx -v Y=yyyy))
# $(eval $(call use-template,globals,globals))
$(eval $(call make-obj,misc,types))
$(eval $(call make-obj,globals,misc))
$(eval $(call make-obj,ipile,misc))

$(eval $(call make-obj,test,globals))

-include .depend

ALLSRCSRC = $(shell find lit/* tpt/* -name '*.c' -or -name '*.h')
ALLSRC    = $(ALLSRCSRC:%=gen/%)
 
tags: ${ALLSRCSRC}
	echo "ALLSRC=" $(ALLSRC)
	ctags ${ALLSRCSRC}

clean:
	rm -rf gen obj exe .depend tags *.pile

#.depend: $(ALL)
#	$(shell find gen -name "*.c" - | while read C; do grep -Po '^#include "\K[^"]*' $C | sed "s#^#$C: #"; done > .depend)

.SECONDARY: 

