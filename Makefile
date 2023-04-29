CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pthread
SRC = proj2.c helper_functions.c
OBJ = $(SRC:.c=.o)
TARGET = proj2

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c helper_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)