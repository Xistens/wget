CC = gcc
WGET_HEADERS = wget.h http.h utils.h connect.h
OBJECTS = http.o connect.o utils.o
CFLAGS = -Wall
WGETLIBS = http.o connect.o utils.o
OBJECTS = http.o wget.o utils.o connect.o

# Build wget
default: utils http connect wget clean

utils: utils.c
	$(CC) -o utils.o -c utils.c

http: http.c utils.h
	$(CC) -o http.o -c http.c

connect: connect.c utils.h
	$(CC) -o connect.o -c connect.c 

wget.o: wget.c $(WGET_HEADERS)
	$(CC) -c wget.c -o wget.o
wget: wget.o
	$(CC) $(CFLAGS) wget.o -o wget $(WGETLIBS)

clean:
	-rm -f $(OBJECTS)
