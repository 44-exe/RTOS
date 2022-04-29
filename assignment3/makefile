# comment
# (note: the <tab> in the command line is necessary for make to work) 
#target:  dependency1 dependency2 ...
#    <tab> command

#for example:
#
# target entry to build program executable from program and mylib 
# object files 
#

all: clean build

build:
	gcc Assignment3_template_Prg_1.c -o Assignment3_Prg1 -lpthread -lrt -Wall

clean: 
	$(RM) Assignment3_Prg1
	$(RM) output.txt
