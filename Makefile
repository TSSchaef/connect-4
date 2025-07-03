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

wasm: src/connect-4.c src/game.c src/solver.c src/table.c
	emcc src/connect-4.c src/game.c src/solver.c src/table.c \
		-g \
		-s WASM=1 \
		-s USE_SDL=2 \
		-s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" \
		-s USE_SDL_TTF=2 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s ASSERTIONS=2 \
		-s EXPORTED_FUNCTIONS="['_main']" \
		-s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','FS']" \
		--preload-file src/images \
		-o connect-4.html

.PHONY: clean

clean:
	rm -f *.o core* connect-4 connect-4.* valgrind* *.wasm *.js
