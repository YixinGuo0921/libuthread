# Target librariy
CC = gcc

# General gcc options
CFLAGS	:= -Wall -Wextra -Werror

## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

SRC=$(wildcard *.c)

OBJS=$(patsubst %.c,%.o,$(SRC))

MAIN := libuthread.a
all: $(MAIN)

$(MAIN): $(OBJS)
	ar rcs $(MAIN) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.a *.o
