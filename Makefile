output: NewCOR.o Funcs.o 
	gcc -g -fsanitize=address NewCOR.o Funcs.o -o COR 

COR.o: NewCOR.c FUNCS.h
	gcc -g -fsanitize=address -c NewCOR.c 

Funcs.o: Funcs.c 
	gcc -g -fsanitize=address -c Funcs.c 

clean:
	rm *.o COR
