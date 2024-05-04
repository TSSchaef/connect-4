#Makefile

connect-4: connect-4.o 
	g++ connect-4.o  -o connect-4 -lSDL2 -lSDL2_ttf -lSDL2_image

connect-4.o: src/connect-4.cpp
	g++ -Wall -Werror -g src/connect-4.cpp -c

.PHONY: clean

clean:
	rm -f *.o core* connect-4 valgrind*
