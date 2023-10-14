CFLAGS = -Wall -g -lm -pedantic
#CFLAGS = -Wall -lm -o2

PROGRAMS = day1 day2 day3 day4 day5

.PHONY : clean all

all : $(PROGRAMS)

execute : 
	./day1
	./day2
	./day3
	./day4
	./day5
	

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	./$@

clean :
	rm -f *.o $(PROGRAMS)