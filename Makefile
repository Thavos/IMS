# IMS 2023/24
# Auth: Simon Peter Hruz, xhruzs00
# Proj: Traffic Cellular Automata

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -pthread
DEBUG_FLAGS = -g

SOURCE_FILE = traffic-ca.c
TARGET_FILE = traffic-ca

all: $(TARGET_FILE)

$(TARGET_FILE): $(SOURCE_FILE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

valgrind: $(TARGET_FILE)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET_FILE)

debug: $(TARGET_FILE)
	gdb -ex run --args ./$(TARGET_FILE)

clean:
	rm -f $(TARGET_FILE)