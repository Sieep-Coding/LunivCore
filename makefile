CC = gcc
TARGET = lcore

SRC_DIR = src/C
CORE_DIR = $(SRC_DIR)/core
LUA_BIND_DIR = $(SRC_DIR)/lua_bindings

LUA_SCRIPT = src/lua/config.lua

CFLAGS = -Wall -Wextra -I$(SRC_DIR) -I$(LUA_BIND_DIR) -I$(CORE_DIR)
LDFLAGS = -llua -lm

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/lcore_exec.c \
          $(CORE_DIR)/dataset.c \
          $(SRC_DIR)/lcore/ast.c \
          $(SRC_DIR)/lcore/lexer.c \
          $(SRC_DIR)/lcore/parser.c \
          $(SRC_DIR)/lcore/render.c \
          $(LUA_BIND_DIR)/lbind.c

OBJECTS = $(SOURCES:.c=.o)

$(CORE_DIR)/%.o: $(CORE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/lcore/%.o: $(SRC_DIR)/lcore/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LUA_BIND_DIR)/%.o: $(LUA_BIND_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


$(TARGET): $(OBJECTS)
	# Recipe to link the final executable (MUST be indented with a TAB)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

all: $(TARGET)
	# This target simply depends on $(TARGET)
	
run: $(TARGET)
	# Recipe to execute the program (MUST be indented with a TAB)
	@echo "--- Running $(TARGET) with examples/lua/helloworld.lua ---"
	./$(TARGET) examples/lua/helloworld.lua

clean:
	# Recipe to remove all generated files (MUST be indented with a TAB)
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all run clean