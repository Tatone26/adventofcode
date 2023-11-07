# ADVENTOFCODE 2021

## How to make any file work

1. Make sure there is a dayX.txt file in the inputs folder corresponding to the day you want to run
2. Use "make dayX" to compile and run the executable.

> To use any other functions of this makefile, there must be every .txt files necessary to run the current programs.
> Or change the PROGRAMS variable in the makefile to select only the interesting ones

## Othere functions

1. "make execute" launches every programs with "time output/dayX" one after the other.
2. "make time" executes every programs and show the total time (with the shell time keyword).
3. "make clean" removes the output folder and every other things that could have been created.

