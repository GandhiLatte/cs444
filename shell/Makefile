OBJS=exec.o main.o parser.o syscalls.o tok.o types.o

SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)

ifeq ($(XV6),1)
sh: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -Wl,-r -o sh.o $(OBJS)
else
sh: $(SRCS) $(HDRS)
	$(CC) -std=gnu99 -D_XOPEN_SOURCE=700 -ggdb -O0 -o sh $(SRCS)
endif

clean:
	rm -f sh sh.o $(OBJS) *.d
