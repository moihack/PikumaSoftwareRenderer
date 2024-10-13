build:
	gcc -Wall -std=c99 ./src/*.c -I/usr/include/SDL2 -lSDL2 -lm -o renderer

run:
	./renderer

clean:
	rm renderer

