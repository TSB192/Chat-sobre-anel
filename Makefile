output: NewCOR.o Funcs.o 
	gcc -g NewCOR.o Funcs.o -o COR 

COR.o: NewCOR.c FUNCS.h
	gcc -g -c NewCOR.c 

Funcs.o: Funcs.c 
	gcc -g -c Funcs.c 

clean:
	rm *.o COR
