SHELL := /bin/bash

CFLAGS = -g -Wall -Wpedantic -Wextra -lm -pedantic
#CFLAGS = -Wall -lm -o3 -pedantic

OUTFOLDER = ./output# folder with binaries
INPUTFOLDER = ./input# folder with text inputs
CODEFOLDER = ./code# folder with code

# list of all days done
DAYS = 1 2 3 4 5 6 7 8 9 10
# list of all missed days
NOT_DONE_DAYS = 
# day currently working on
CURRENTDAY = 9

.PHONY : clean all runall valgrind

# compiles all days
all : $(foreach d, $(DAYS), $(OUTFOLDER)/$(d))

# compiles and runs the current day
run : $(OUTFOLDER)/$(CURRENTDAY)
	$<

# run valgrind on the current day
valgrind : $(OUTFOLDER)/$(CURRENTDAY)
	valgrind $<

# run all days (not useful for now)
runall : $(foreach d, $(DAYS), $(OUTFOLDER)/$(d))
	time ($(foreach d, $(DAYS), $(OUTFOLDER)/$(d) ;))
	@echo -e "\n\033[4m"Days missed"\033[m" :
	@echo $(foreach d, $(NOT_DONE_DAYS), $(d) )

.PRECIOUS: $(OUTFOLDER)/%.o # forcing Make to keep all .o files
# .o files
$(OUTFOLDER)/%.o : $(CODEFOLDER)/%.c Makefile
	gcc $(CFLAGS) -o $@ -c $< 

# binaries files
$(OUTFOLDER)/% : $(OUTFOLDER)/%.o $(OUTFOLDER)/runner.o
	gcc $(CFLAGS) -o $@ $^ 

clean :
	rm -f -r *.o ${OUTFOLDER}/*
	clear