SHELL := /bin/bash

CFLAGS = -Wall -Wpedantic -Wextra -g -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

PROGRAMS = day1 day2 day3 day4 day5 day6 day7 day8 day9 day10 day11 day13
# TODO: day12

.PHONY : clean all execute time

all : $(PROGRAMS)

execute : 
	make all
	$(foreach x, $(PROGRAMS), time ./$x; echo ;)

time :
	make all
	time ( $(foreach x, $(PROGRAMS), ./$x;) )

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	time ./$@

clean :
	rm -f *.o $(PROGRAMS)