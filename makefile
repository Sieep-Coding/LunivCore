CC = gcc
TARGET = bi_tool
SRC_DIR = src/C
LUA_SCRIPT = src/lua/report.lua
CFLAGS = -Wall -Wextra
LDFLAGS = -llua -lm

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
run: $(TARGET)
	@echo "--- Running $(TARGET) with $(LUA_SCRIPT) ---"
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean