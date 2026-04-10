C := $(shell find . -name '[a-z]*\.c' -printf '%P\n')
H := $(shell find . -name '[a-z]*\.h' -printf '%P\n')

run: tags bin/test bin/hive
	rm -f core.*
	rm -f *.pile
	stdbuf --output=L bin/test && stdbuf --output=L bin/hive

bin/test: test lib h
	${MAKE} -C test ../bin/test

bin/hive: hive lib h
	${MAKE} -C hive ../bin/hive

tags: ${H} ${C}
	ctags ${H} ${C} 

clean:
	rm -rf tmp bin tags

