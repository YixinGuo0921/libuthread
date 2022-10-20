# Target librariy
CC			=  gcc
override CFLAGS 	+= -Wall\
			   -Wextra\
			   -Werror

OBJS			=  queue.o

MAIN 			:= libuthread.a

all: $(MAIN)

$(MAIN): $(OBJS)
	ar -r $(MAIN) $(OBJS)

$(OBJS): queue.c queue.h
	$(CC) $(CFLAGS) -c $< -o $@
