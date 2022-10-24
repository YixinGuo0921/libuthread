# Target librariy
CC = gcc

override CFLAGS += \
	-Wall \
	-Wextra\
	-Werror

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
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.a *.o
