CFLAGS = -Wall -Wpedantic -Wextra -g -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

PROGRAMS = measurer day1 day2 day3 day4 day5 day6 day7 day8 day9 day10 day11

.PHONY : clean all

all : $(PROGRAMS)

execute : 
	./day1
	./day2
	./day3
	./day4
	./day5
	./day6
	./day7
	./day8
	./day9
	./day10
	./day11
# those ones are too slow and need to be redone

time :
	./measurer

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	./$@

clean :
	rm -f *.o $(PROGRAMS)