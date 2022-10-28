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

OBJS = \
	queue.o \
	uthread.o \
	context.o \
	preempt.o

MAIN := libuthread.a

all: $(MAIN)

$(MAIN): $(OBJS)
	ar rcs $(MAIN) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.a *.o
