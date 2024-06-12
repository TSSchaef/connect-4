#Makefile

connect-4: connect-4.o game.o solver.o table.o
	gcc connect-4.o game.o solver.o table.o -o connect-4 -lSDL2 -lSDL2_ttf -lSDL2_image

connect-4.o: src/connect-4.c src/game.h
	gcc -Wall -Werror -g src/connect-4.c -c

game.o: src/game.c src/game.h
	gcc -Wall -Werror -g src/game.c -c

solver.o: src/solver.c src/solver.h src/table.h
	gcc -Wall -Werror -g src/solver.c -c

table.o: src/table.c src/table.h 
	gcc -Wall -Werror -g src/table.c -c

.PHONY: clean

clean:
	rm -f *.o core* connect-4 valgrind*
