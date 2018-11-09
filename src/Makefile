CC = gcc
HEADERS = wget.h http.h
OBJECTS = http.o
CFLAGS = -Wall
WGETLIBS = http.o
OBJECTS = http.o wget.o

# Build wget
default: http wget clean

http: http.c
	$(CC) -o http.o -c http.c

wget.o: wget.c $(HEADERS)
	$(CC) -c wget.c -o wget.o
wget: wget.o
	$(CC) $(CFLAGS) wget.o -o wget $(WGETLIBS)

clean:
	-rm -f $(OBJECTS)
