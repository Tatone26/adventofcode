CFLAGS = -Wall -g -pedantic
#CFLAGS = -Wall -o2

PROGRAMS = day1 day2

.PHONY : clean all

all : $(PROGRAMS)

execute : 
	./day1
	./day2
	

% : %.c
	gcc $(CFLAGS) -o $@ $<
	./$@

clean :
	rm -f *.o $(PROGRAMS)