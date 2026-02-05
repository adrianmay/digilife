hive: src/hive.c Makefile
	gcc -o hive src/hive.c

tests: src/roulette.c test/test_roulette.c
	gcc  -o test.exe  src/roulette.c test/test_roulette.c
	./test.exe > tests 
