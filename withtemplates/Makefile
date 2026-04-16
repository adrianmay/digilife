CC = gcc
CFLAGS = -g -iquote gen/h -MMD -MP -Wall -Werror


obj/%.o: gen/%.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

lits   = $(shell find lit -type f)
dits   = $(lits:lit/%=gen/%)
gen/%: lit/%
	mkdir -p $(dir $@)
	cp $^ $@

all: $(dits)
	echo $(dits)
###########################################################
define use-template
$(2)ts   = $$(wildcard tpt/$(1)/*)
$(2)hcs  = $$($(2)ts:tpt/$(1)/%=gen/$(2)/%)
gen/$(2)/%: Makefile
gen/$(2)/%: tpt/$(1)/% 
	echo $$($(2)hcs)
	mkdir -p gen/$(2)
	awk $(3) '{ gsub(/XX/,X); gsub(/YY/,Y); }1' $$^ > $$@
all: $$($(2)hcs) gen/h/$(2)_$(1).h
all: obj/$(2)_$(1).o
obj/$(2)_$(1).o: obj/$(2)/$(1).o # This will be objcopy with limited exports
	ln -fs ../$$^ $$@
gen/h/$(2)_$(1).h: gen/$(2)/$(1).h
	mkdir -p gen/h
	ln -fs ../$$^ $$@
endef
###########################################################
define make-obj
$(1)C   := $$(wildcard gen/$(1)/*.c)
$(1)OBJ := $$($(1)C:gen/%.c=obj/%.o)
$(1)DEP := $$($(1)OBJ:.o=.d)
-include $$($(1)DEP)
blah:
	echo $$($(1)C)
	echo $$($(1)OBJ)
endef
###########################################################
$(eval $(call use-template,pile,foo,-v X=xxxx -v Y=yyyy))
$(eval $(call use-template,globals,globals))
$(eval $(call make-obj,globals))


clean:
	rm -rf gen obj


