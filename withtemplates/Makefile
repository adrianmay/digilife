CC = gcc
CFLAGS = -g -iquote h -MMD -MP -Wall -Werror

define use-template
$(2)ts   = $$(wildcard tpt/$(1)/*)
$(2)hcs  = $$($(2)ts:tpt/$(1)/%=gen/$(2)/%)

gen/$(2)/%: Makefile

gen/$(2)/%: tpt/$(1)/% 
	echo $$($(2)ts)
	mkdir -p gen/$(2)
	awk $(3) '{ gsub(/XX/,X); gsub(/YY/,Y); }1' $$^ > $$@

all: $$($(2)hcs)
endef

$(eval $(call use-template,pile,foo,-v X=xxxx -v Y=yyyy))

clean:
	rm -rf gen


