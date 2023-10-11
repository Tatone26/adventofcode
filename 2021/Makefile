CFLAGS = -Wall -g -lm -pedantic
#CFLAGS = -Wall -lm -o2

PROGRAMS = day1 day2 day3

.PHONY : clean all

all : $(PROGRAMS)

execute : 
	./day1
	./day2
	./day3
	

% : %.c %.txt
	gcc  $< -o $@  $(CFLAGS)
	./$@

clean :
	rm -f *.o $(PROGRAMS)