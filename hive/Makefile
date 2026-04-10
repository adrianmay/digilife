C := $(shell find . -name '[a-z]*\.c' -printf '%P\n')
H := $(shell find . -name '[a-z]*\.h' -printf '%P\n')

run: bin/test bin/hive
	stdbuf --output=L bin/test && stdbuf --output=L bin/hive

bin/test:
	${MAKE} -C test ../bin/test

bin/hive:
	${MAKE} -C hive ../bin/hive

tags: ${H} ${C}
	ctags ${H} ${C} 

clean:
	rm -rf tmp bin tags
