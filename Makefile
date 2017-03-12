all: clean test ceptr
.PHONY: all

CEPTR_SRC_FILES := $(wildcard src/*.h src/*.c)
SPECS_SRC_FILES := $(wildcard src/*.h src/*.c spec/*.c)
SPECS_SRC_FILES := $(filter-out src/ceptr.c, $(SPECS_SRC_FILES))

ceptr: $(CEPTR_SRC_FILES)
	gcc -pthread -g -o ceptr $(CEPTR_SRC_FILES)

test: ceptr_specs
	./ceptr_specs

ceptr_specs: $(SPECS_SRC_FILES)
	gcc -pthread -g -o ceptr_specs $(SPECS_SRC_FILES)

clean:
	-rm -rf ceptr_specs *.o ceptr_spec.dSYM ceptr ceptr.dSYM #src/base_defs.c src/base_defs.h

#base_defs.h: src/base_defs
#	perl src/base_defs.pl

basedef.yy.c: basedef.l
	flex -o basedef.yy.c basedef.l 

basedef.tab.c: basedef.y
	bison -d --debug basedef.y

basedef: basedef.yy.c basedef.tab.c
	cc basedef.yy.c basedef.tab.c -o basedef -ll

semclean:
	rm basedef basedef.yy.? basedef.tab.?

