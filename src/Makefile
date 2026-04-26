CC     = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c reports.c filter.c permissions.c log.c
OBJS = $(SRCS:.c=.o)
TARGET = city_manager

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)