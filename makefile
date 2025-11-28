CC = gcc
TARGET = bi_tool

SRC_DIR = src/C
CORE_DIR = src/C/core

LUA_SCRIPT = src/lua/config.lua

CFLAGS = -Wall -Wextra -I$(SRC_DIR)
LDFLAGS = -llua -lm 

SOURCES = $(wildcard $(SRC_DIR)/*.c) \
          $(wildcard $(CORE_DIR)/*.c) \
          $(wildcard $(SRC_DIR)/lcore/*.c)


OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

run: $(TARGET)
	@echo "--- Running $(TARGET) ---"
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all run clean
