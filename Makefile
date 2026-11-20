CC ?= gcc
CFLAGS ?= -O3 -Wall -Wextra -std=c11 -Iinclude
TARGET := build/cjangd
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=build/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build
