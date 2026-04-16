CC = gcc
CFLAGS = -g -iquote . -iquote .. -MMD -MP -Wall -Werror

OBJDIR   = obj

${OBJDIR}/%.o: gen/%.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

###########################################################
define use-template
$(2)ts   = $$(wildcard tpt/$(1)/*)
$(2)hcs  = $$($(2)ts:tpt/$(1)/%=gen/$(2)/%)
gen/$(2)/%: Makefile
gen/$(2)/%: tpt/$(1)/% 
	echo $$($(2)hcs)
	mkdir -p gen/$(2)
	awk $(3) '{ gsub(/XX/,X); gsub(/YY/,Y); }1' $$^ > $$@
all: $$($(2)hcs) gen/$(2)_$(1).h obj/$(2)_$(1).o
obj/$(2)_$(1).o: obj/$(2)/$(1).o
	ln -fs ../$$^ $$@
gen/$(2)_$(1).h: gen/$(2)/$(1).h
	ln -fs ../$$^ $$@
endef
###########################################################
define make-obj
$(1)C   := $$(wildcard gen/$(1)/*.c)
$(1)OBJ := $$($(1)C:gen/%.c=$${OBJDIR}/%.o)
$(1)DEP := $$($(1)OBJ:.o=.d)
-include $$($(1)DEP)
blah:
	echo $$($(1)C)
	echo $$($(1)OBJ)
endef
###########################################################
$(eval $(call use-template,pile,foo,-v X=xxxx -v Y=yyyy))

$(eval $(call use-template,globals,globals,-v X=xxxx -v Y=yyyy))
$(eval $(call make-obj,globals))


clean:
	rm -rf gen obj


