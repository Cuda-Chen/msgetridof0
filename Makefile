CC = gcc
EXEC = msgetridof0
COMMON = -I/usr/local -I.
CFLAGS = -Wall
LDFLAGS = -L/usr/local
LDLIBS = -lmseed -lm

OBJS = main.o get_rid_of_zero.o

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(EXEC)
