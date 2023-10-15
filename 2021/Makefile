CFLAGS = -Wall -g -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

PROGRAMS = day1 day2 day3 day4 day5 day6 day7

.PHONY : clean all

all : $(PROGRAMS)

execute : 
	./day1
	./day2
	./day3
	./day4
	./day6
	./day7
#	./day5
# those ones are too slow.

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	./$@

clean :
	rm -f *.o $(PROGRAMS)