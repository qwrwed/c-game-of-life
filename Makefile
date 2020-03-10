gameoflife: gameoflife.o
	gcc -Wall -Wextra -pedantic -std=c11 -o gameoflife gameoflife.o ./libgol.so

gol.o: gol.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -fPIC -c gol.c

gameoflife.o: gameoflife.c libgol.so
	gcc -Wall -Wextra -pedantic -std=c11 -c gameoflife.c

libgol.so: gol.o gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -o libgol.so -shared gol.o 

clean:
	rm -rf gameoflife gol.o gameoflife.o libgol.so
