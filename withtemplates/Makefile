# ./compile is a hack
# Test that it rebuilds what it should
# What's going on with all the deps going to make-exe as well
# Consolidate some functions

CFLAGS = -g -iquote gen/h -Wall -Werror
CC = gcc $(CFLAGS)

###########################################################
# Copy lits to gen verbatim
#
#

# lits   = $(shell find lit -type f)
# dits   = $(lits:lit/%=gen/%)
# 
# gen/%: lit/%
# 	mkdir -p $(dir $@)
# 	cp $^ $@

all: tags gensrc compile link # exes
compile: gensrc
link: compile

PHONY: gensrc compile link
###########################################################
# Build any .c under gen to the corresponding .o under obj

define make-obj

# $(1)C = $$(wildcard gen/$(1)/*.c)
# $(1)H = $$(wildcard gen/$(1)/*.h)
# $(1)OBJ  = $$($(1)C:gen/%.c=obj/%.o)
$(1)HHD  = $(2) # Dunno why I need this step
$(1)HDEP = $$($(1)H) $$($(1)HHD:%=gen/h/%.h)
$(1)OOD  = $(3) # Dunno why I need this step
$(1)ODEP = $$($(1)OOD:%=obj/%.o)

# obj/$(1)/%.o: gen/$(1)/%.c | $$($(1)HDEP)
# 	mkdir -p $$(dir $$@)
# 	$$(CC) -c $$< -o $$@

obj/$(1).o: $$(wildcard gen/$(1)/*.c) gen/h/$(1).h $$($(1)HDEP) $$($(1)ODEP)
	./compile $(1) "$$(wildcard gen/$(1)/*.c)" "$$($(1)ODEP)"

gen/h/$(1).h: gen/$(1)/$(1).h # Export the main header
	mkdir -p gen/h
	ln -fs ../../$$^ $$@

# obj/$(2)_$(1).o: gen/h/$(2)_$(1).h # Provide a publicly visible version of the obj

all: obj/$(1).o

endef

###########################################################

define make-exe

# $(1)ODS = $(2)
# $(1)ODEP = $$($(1)ODS:%=obj/%.o)

bin/$(1): obj/$(1).o # $$($(1)ODEP) # | gen/h/$(2).h
	mkdir -p bin
	$$(CC) $$^ -o $$@

link: bin/$(1)

endef


###########################################################

define use-lit

$(1)TS = $$(wildcard lit/$(1)/*) # All the files in the template - should be a find
$(1)GS = $$($(1)TS:lit/%=gen/%) # Corresponding paths in the module

gen/$(1)/%: lit/$(1)/%  # Copy and apply replacements
	mkdir -p gen/$(1)
	cp $$^ $$@

gensrc: $$($(1)GS)

endef

###########################################################
# Params are template name, generated module name, replacements
define use-template

$(2)_$(1)TS = $$(wildcard tpt/$(1)/*) # All the files in the template - should be a find
$(2)_$(1)GS = $$($(2)_$(1)TS:tpt/$(1)/%=gen/$(2)_$(1)/$(2)_%) # Corresponding paths in the module

gen/$(2)_$(1)/$(2)_%: tpt/$(1)/%  # Copy and apply replacements
	mkdir -p gen/$(2)_$(1)
	awk $(3) '{ gsub(/XX/,X); gsub(/YY/,Y); }1' $$^ > $$@

gensrc: $$($(2)_$(1)GS)
# gen/$(2)/%: Makefile # Dunno if this is working

# all: $$($(2)GS) gen/h/$(2)_$(1).h # Kick make into requiring the gen file. Should be obsolete if the gen file is required by something else

# all: obj/$(2)_$(1).o # Kick make into making the public obj

endef
###########################################################
$(eval $(call use-lit,h))
$(eval $(call use-lit,misc))
$(eval $(call use-lit,globals))
$(eval $(call use-lit,ipile))
$(eval $(call use-lit,test))

$(eval $(call make-obj,misc,types))
$(eval $(call make-obj,globals,misc))
$(eval $(call make-obj,ipile,misc))

$(eval $(call use-template,pile,Junk,-v X=Junk -v Y=GIGA))
$(eval $(call make-obj,Junk_pile,ipile types,))

$(eval $(call make-obj,test,globals misc Junk_pile, ipile misc globals Junk_pile))
$(eval $(call make-exe,test))

-include .depend

ALLSRCSRC = $(shell find lit/* tpt/* -name '*.c' -or -name '*.h')
ALLSRC    = $(ALLSRCSRC:%=gen/%)
 
tags: ${ALLSRCSRC}
	ctags ${ALLSRCSRC}

clean:
	rm -rf gen obj bin .depend tags *.pile

#.depend: $(ALL)
#	$(shell find gen -name "*.c" - | while read C; do grep -Po '^#include "\K[^"]*' $C | sed "s#^#$C: #"; done > .depend)

.SECONDARY: 

