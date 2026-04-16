CC = gcc
CFLAGS = -g -iquote h -MMD -MP -Wall -Werror

define use-template
$(1)tpt  = $(shell cat mod/$(1)/tpt)
$(1)ts   = $$(wildcard tpt/$$($(1)tpt)/*)
$(1)hcs  = $$($(1)ts:tpt/$$($(1)tpt)/%=gen/$(1)/%)
$(1)X    = $(shell cat mod/$(1)/X)

gen/$(1)/%: $(1)/mod/X

gen/$(1)/%: tpt/$$($(1)tpt)/%
	echo $$($(1)hcs)
	mkdir -p gen/$(1)
	awk -v X=$$($(1)X) '{gsub(/XX/,X)}1' $$^ > $$@

all: $$($(1)hcs) # gen/foo/pile.h
endef

$(eval $(call use-template,foo))

clean:
	rm -rf gen

