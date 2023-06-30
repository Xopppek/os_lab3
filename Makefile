CFLAGS=-Wall

main:
	gcc -pthread -o main main.c

clean:
	rm -f main
	rm -f data.txt
