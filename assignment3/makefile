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

build: Prg_1 Prg_2

Prg_1:
	gcc Assignment3_template_Prg_1.c -o Assignment3_Prg1 -lpthread -lrt -Wall

Prg_2:
	gcc Assignment3_template_Prg_2.c -o Assignment3_Prg2 -lpthread -lrt -Wall

clean: 
	$(RM) Assignment3_Prg1 Assignment3_Prg2
	$(RM) output.txt
