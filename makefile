all: hello

hello: encode.o

spectrum: spectrum.o

encode.o: encode.c
	gcc encode.c -o encode

clean:
	rm -rf *o hello
