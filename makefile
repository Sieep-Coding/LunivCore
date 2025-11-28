# --- Variables ---
CC = gcc
TARGET = bi_tool
SRC_DIR = src/C
LUA_SCRIPT = src/lua/report.lua
CFLAGS = -Wall -Wextra
LDFLAGS = -llua -lm

# Find all C source files in the SRC_DIR
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

# The default target: builds the executable
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to run the tool and the Lua script
run: $(TARGET)
	@echo "--- Running $(TARGET) with $(LUA_SCRIPT) ---"
	./$(TARGET)

# Rule to clean up compiled files
clean:
	rm -f $(TARGET)

# Phony targets don't correspond to actual files
.PHONY: all run clean