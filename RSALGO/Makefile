CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lcjson

SRCS = main.c graph.c queue.c traversal.c json_utils.c
OBJS = $(SRCS:.c=.o)
TARGET = graph_program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean