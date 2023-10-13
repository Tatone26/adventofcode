CFLAGS = -Wall -g -lm -pedantic
#CFLAGS = -Wall -lm -o2

PROGRAMS = day1 day2 day3 day4

.PHONY : clean all

all : $(PROGRAMS)

execute : 
	./day1
	./day2
	./day3
	./day4
	

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	./$@

clean :
	rm -f *.o $(PROGRAMS)