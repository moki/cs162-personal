CC = gcc
CFLAGS = -g -Wall

HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGETS = $(SOURCES:.c=.out)

all: $(TARGETS)

%.out : %.o
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

test-wc: wc.out
	./test-wc.sh

clean:
	-rm -f *.o
	-rm -f *.out

