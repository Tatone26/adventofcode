SHELL := /bin/bash

CFLAGS = -Wall -Wpedantic -Wextra -g -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

OUTFOLDER = ./output# folder with binaries
INPUTFOLDER = ./input# folder with text inputs
CODEFOLDER = ./code# folder with code

# list of all days done
DAYS = 1
# list of all missed days
NOT_DONE_DAYS = 
# day currently working on
CURRENTDAY = 1

.PHONY : clean all runall valgrind

# compiles and runs the current day
run : $(OUTFOLDER)/$(CURRENTDAY)
	$<

# compiles all days
all : $(foreach d, $(DAYS), $(OUTFOLDER)/$(d))

# run valgrind on the current day
valgrind : $(OUTFOLDER)/$(CURRENTDAY)
	valgrind $<

# run all days (not useful for now)
runall : $(foreach d, $(DAYS), $(OUTFOLDER)/$(d))
	$<

.PRECIOUS: $(OUTFOLDER)/%.o # forcing Make to keep all .o files
# .o files
$(OUTFOLDER)/%.o : $(CODEFOLDER)/%.c
	gcc $(CFLAGS) -o $@ -c $< 

# binaries files
$(OUTFOLDER)/% : $(OUTFOLDER)/%.o $(OUTFOLDER)/utils.o
	gcc $(CFLAGS) -o $@ $^ 

clean :
	rm -f -r *.o ${OUTFOLDER}/*
	clear