# comment
# (note: the <tab> in the command line is necessary for make to work) 
#target:  dependency1 dependency2 ...
#      <tab> command

#for example:
#
# target entry to build program executable from program and mylib 
# object files 
#

all: clean build

build:
	gcc assignment2.c -o assignment2 -lpthread -lrt -Wall

clean: 
	  $(RM) assignment2
	  $(RM) output.txt
