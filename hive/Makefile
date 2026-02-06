
all: hive

# Turn .h.m4 into .m4.h, watching pile.m4 which it includes
hive.m4.h:	hive.h.m4 pile.m4 Makefile
	m4 hive.h.m4 > hive.m4.h

# Turn .c.m4 into .m4.c, watching .m4.h which it includes
hive.m4.c:	hive.c.m4 hive.m4.h Makefile
	m4 hive.c.m4 > hive.m4.c 

# Compile and link .m4.c, .c and pile.c
hive: hive.m4.c hive.c pile.c Makefile
	gcc -o hive hive.m4.c hive.c pile.c



