SHELL := /bin/bash

CFLAGS = -g -Wall -Wpedantic -Wextra -pedantic -DNDEBUG
#CFLAGS = -O3 -march=native -funroll-loops -flto -fomit-frame-pointer -ffast-math -fno-stack-protector

OUTFOLDER = ./output# folder with binaries
INPUTFOLDER = ./input# folder with text inputs
CODEFOLDER = ./code# folder with code

# list of all days done
DAYS = 1 2 3 4 5 6
# list of all missed days
NOT_DONE_DAYS =
# day currently working on
DAY = 6

.PHONY : clean all runall valgrind

# compiles all days
all : $(foreach d, $(DAYS), $(OUTFOLDER)/$(d))

# compiles and runs the current day
run : $(OUTFOLDER)/$(DAY)
	$< $(INPUTFOLDER)/$(DAY).txt

# compiles and runs the current day with foo.txt
test :$(OUTFOLDER)/$(DAY)
	$< $(INPUTFOLDER)/foo.txt

# run valgrind on the current day
valgrind : $(OUTFOLDER)/$(DAY)
	valgrind $< $(INPUTFOLDER)/$(DAY).txt

# run all days
runall : $(foreach d, $(DAYS), $(OUTFOLDER)/$(d))
	time ($(foreach d, $(DAYS), $(OUTFOLDER)/$(d) $(INPUTFOLDER)/$(d).txt;))
	@echo -e "\n\033[4m"Days missed"\033[m" :
	@echo $(foreach d, $(NOT_DONE_DAYS), $(d) )

.PRECIOUS: $(OUTFOLDER)/%.o # forcing Make to keep all .o files
# .o files
$(OUTFOLDER)/%.o : $(CODEFOLDER)/%.c Makefile
	gcc $(CFLAGS) -o $@ -c $< -lm

# binaries files
$(OUTFOLDER)/% : $(OUTFOLDER)/%.o $(OUTFOLDER)/runner.o
	gcc $(CFLAGS) -o $@ $^ -lm

clean :
	rm -f -r *.o ${OUTFOLDER}/* *.out* vgcore* 
	clear