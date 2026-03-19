
all: hive
	./hive

define prog_tmpl
tmp/$(1).m4.h:	$(1).h.m4 pile.m4 Makefile tmp pile.h $(2)
	[ -z $(2) ] || ./$(2) || { echo "Precondition '$(2)' failed"; false; }
	m4 $(1).h.m4 > tmp/$(1).m4.h

tmp/$(1).m4.c:	$(1).c.m4 tmp/$(1).m4.h Makefile pile.h
	m4 -Itmp -I. $(1).c.m4 > tmp/$(1).m4.c 

$(1): tmp/$(1).m4.c $(1).c pile.c Makefile pile.h
	gcc -g -Itmp -I. -o $(1) tmp/$(1).m4.c $(1).c pile.c
endef

$(eval $(call prog_tmpl,test))
$(eval $(call prog_tmpl,hive,test))

tmp: 
	mkdir tmp

clean: 
	rm -rf tmp test hive *.pile

