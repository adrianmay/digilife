CFLAGS = -g -iquote gen/h -iquote gen -Wall -Werror
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

all: $(dits) obj/test.o obj/globals.o
	echo "Doing all"

# all: $(gits)
# 	echo $(gits)

###########################################################
# Build any .c under gen to the corresponding .o under obj
obj/%.o: gen/%.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

define make-obj

$(1)LIT  = $$(wildcard lit/$(1)/*.c)
$(1)C    = $$($(1)LIT:lit/%=gen/%)
$(1)OBJ  = $$($(1)C:gen/%.c=obj/%.o)

obj/$(1).o: $$($(1)OBJ) # Provide a publicly visible version of the obj
	echo $$($(1)LIT)
	mkdir -p obj
	ld --relocatable -o $$@ $$^

gen/h/$(1).h: gen/$(1)/$(1).h # Export the main header
	mkdir -p gen/h
	ln -fs $$^ $$@

obj/$(2)_$(1).o: gen/h/$(2)_$(1).h # Provide a publicly visible version of the obj

endef

###########################################################

define make-exe


$(1): obj/$(1).o obj/$(2).o | gen/h/$(2).h
	$$(CC) -g -O0 $$^ -o $$@

all: $(1)

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
$(eval $(call make-obj,globals))
$(eval $(call make-obj,test,globals))
$(eval $(call make-exe,test,globals))

-include .depend

clean:
	rm -rf gen obj


