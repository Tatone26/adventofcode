CFLAGS = -Wall -g -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

PROGRAMS = day1 day2 day3 day4 day5 day6 day7 day8 day9

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
# those ones are too slow and need to be redone

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	./$@

clean :
	rm -f *.o $(PROGRAMS)