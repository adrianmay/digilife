
all: hive

tmp: 
	mkdir tmp

# Turn .h.m4 into .m4.h, watching pile.m4 which it includes
tmp/hive.m4.h:	hive.h.m4 pile.m4 Makefile tmp
	m4 hive.h.m4 > tmp/hive.m4.h

# Turn .c.m4 into .m4.c, watching .m4.h which it includes
tmp/hive.m4.c:	hive.c.m4 tmp/hive.m4.h Makefile
	m4 -Itmp -I. hive.c.m4 > tmp/hive.m4.c 

# Compile and link .m4.c, .c and pile.c
hive: tmp/hive.m4.c hive.c pile.c Makefile
	gcc -Itmp -I. -o hive tmp/hive.m4.c hive.c pile.c

clean: 
	rm -rf tmp hive *.pile

