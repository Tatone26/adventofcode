SHELL := /bin/bash

CFLAGS = -Wall -Wpedantic -Wextra -g -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

PROGRAMS = day1 day2 day3 day4 day5 day6 day7 day8 day9 day10 day11 day13 day14 day15
# TODO: day12
OUTFOLDER = output

VPATH = .:./output:./inputs
.SUFFIXES: 

.PHONY : clean all execute time

all : $(PROGRAMS)

execute : 
	make all
	$(foreach x, $(PROGRAMS), time $(OUTFOLDER)/$x; echo ;)

time :
	make all
	time ( $(foreach x, $(PROGRAMS), $(OUTFOLDER)/$x;) )

valgrind :
	make all
	$(foreach x, ${PROGRAMS}, valgrind -q $(OUTFOLDER)/$x;)

% : %.c %.txt
	mkdir -p $(OUTFOLDER)
	gcc  $< -o ./$(OUTFOLDER)/$@ $(CFLAGS)
	time ./$(OUTFOLDER)/$@

% : %.c
	test -s ./inputs/$@.txt || { echo "Didn't found file $@.txt. Creating empty one."; touch ./inputs/$@.txt ; make $@ ; }


clean :
	rm -f -r *.o output *.out $(PROGRAMS) *.out.*