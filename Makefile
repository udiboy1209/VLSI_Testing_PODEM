CC=g++
CFLAGS=-g
INCLUDES=
OBJS= build/setutil.o\
      build/gates.o \
      build/simulation.o\
      build/podem.o\
      build/parser.tab.o \
      build/parser.yy.o

HEADERS= setutil.h\
	 gates.h\
	 simulation.h\
	 podem.h

all: atpg

atpg: $(OBJS) build/atpg.o $(HEADERS)
	$(CC) -o atpg $(OBJS) build/atpg.o -lfl

deductive: $(OBJS) build/deductive.o $(HEADERS)
	$(CC) -o deductive $(OBJS) build/deductive.o -lfl

build/%.tab.o: build/%.tab.c
	$(CC) -c $< -o $@ -I.

build/%.tab.c build/%.tab.h: %.y
	bison -d -b "build/$*" $<

build/%.yy.o: build/%.yy.c
	$(CC) -c $< -o $@ -lfl

build/%.yy.c: %.l
	flex -o $@ $<

build/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $< 

deductive-test: deductive
	./deductive fulladder.txt
atpg-test: atpg
	./atpg fulladder.txt 2> debug.log | tee output.log

clean:
	rm -vf build/* *.log atpg deductive
