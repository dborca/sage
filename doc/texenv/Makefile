.PHONY: all clean realclean

CC = gcc
CFLAGS += -Wall -W -pedantic -ansi
CFLAGS += -O2

LD = gcc
LDFLAGS = -s
LDLIBS =

SOURCES = \
	lexer.c \
	parser.c \
	tree.c \
	util.c \
	main.c

OBJECTS = $(SOURCES:.c=.o)

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<

all: exu

exu: $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^ $(LDLIBS)

clean:
	-$(RM) $(OBJECTS)

realclean: clean
	-$(RM) exu
