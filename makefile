# Compiler and target
CC = gcc
TARGET = lcore

# Directories
SRC_DIR = src/C
CORE_DIR = $(SRC_DIR)/core
LUA_BIND_DIR = $(SRC_DIR)/lua_bindings
LCORE_DIR = $(SRC_DIR)/lcore

# Lua script (for run target)
LUA_SCRIPT = src/lua/config.lua

# Compiler flags
CFLAGS = -Wall -Wextra -I$(SRC_DIR) -I$(LUA_BIND_DIR) -I$(CORE_DIR)
LDFLAGS = -llua -lm

# Source files (dataset.c must appear before lbind.c)
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/lcore_exec.c \
          $(CORE_DIR)/dataset.c \
          $(SRC_DIR)/lcore/ast.c \
          $(SRC_DIR)/lcore/lexer.c \
          $(SRC_DIR)/lcore/parser.c \
          $(SRC_DIR)/lcore/render.c \
          $(LUA_BIND_DIR)/lbind.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Pattern rules for compilation
$(CORE_DIR)/%.o: $(CORE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/lcore/%.o: $(SRC_DIR)/lcore/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LUA_BIND_DIR)/%.o: $(LUA_BIND_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link target with proper object order
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) \
        $(CORE_DIR)/dataset.o \
        $(filter-out $(CORE_DIR)/dataset.o, $(OBJECTS)) \
        -o $@ $(LDFLAGS)

# Default build
all: $(TARGET)

# Run the program with example Lua script
run: $(TARGET)
	@echo "--- Running $(TARGET) with $(LUA_SCRIPT) ---"
	./$(TARGET) $(LUA_SCRIPT)

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all run clean
