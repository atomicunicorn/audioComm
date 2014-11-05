all: hello

hello: encode.o

encode.o: encode.c
	gcc encode.c -o encode

clean:
	rm -rf *o hello
