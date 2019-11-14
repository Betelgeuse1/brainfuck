CC=gcc

main: main.c
	$(CC) main.c -o main

clear:
	rm main

debug: main.c
	$(CC) -g main.c -o main

run: main
	./main

bf_clean:
	rm -f *.b
