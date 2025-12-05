# Makefile - Enhanced with chart rendering support

# Compiler and target
CC = gcc
TARGET = lcore

# Directories
SRC_DIR = src/C
CORE_DIR = $(SRC_DIR)/core
LUA_BIND_DIR = $(SRC_DIR)/lua_bindings
LCORE_DIR = $(SRC_DIR)/lcore
DP_DIR = $(SRC_DIR)/data_processing

# Example scripts
EXAMPLES_DIR = examples
LUA_EXAMPLES = $(EXAMPLES_DIR)/charts.lua \
               $(EXAMPLES_DIR)/generic_charts.lua \
               $(EXAMPLES_DIR)/dashboard.lua \
               $(EXAMPLES_DIR)/multi_dataset.lua \
               $(EXAMPLES_DIR)/eval_dsl.lua

LCORE_EXAMPLES = $(EXAMPLES_DIR)/mixed.lcore \
                 $(EXAMPLES_DIR)/report.lcore \
                 $(EXAMPLES_DIR)/analytics.lcore

# Compiler flags - Added -lm for math library (required for render.c)
CFLAGS = -Wall -Wextra -I$(SRC_DIR) -I$(LUA_BIND_DIR) -I$(CORE_DIR) -I$(DP_DIR)
LDFLAGS = -llua -lm -lsqlite3 -ljson-c

# Source files (dataset.c must appear before lbind.c)
# render.c added to support chart rendering
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/lcore_exec.c \
          $(CORE_DIR)/dataset.c \
          $(SRC_DIR)/lcore/ast.c \
          $(SRC_DIR)/lcore/lexer.c \
          $(SRC_DIR)/lcore/parser.c \
          $(SRC_DIR)/lcore/render.c \
          $(LUA_BIND_DIR)/lbind.c \
          $(DP_DIR)/dp_dataset.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# ========== Pattern Rules ==========

$(CORE_DIR)/%.o: $(CORE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/lcore/%.o: $(SRC_DIR)/lcore/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LUA_BIND_DIR)/%.o: $(LUA_BIND_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========== Build Targets ==========

# Build executable with proper object order
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) \
	$(CORE_DIR)/dataset.o \
	$(filter-out $(CORE_DIR)/dataset.o, $(OBJECTS)) \
	-o $@ $(LDFLAGS)

# Default build
all: $(TARGET)

# ========== Execution Targets ==========

# Run with basic Lua example
run-lua: $(TARGET)
	@echo "--- Running $(TARGET) with charts.lua ---"
	./$(TARGET) $(EXAMPLES_DIR)/charts.lua

# Run with generic chart example
run-generic: $(TARGET)
	@echo "--- Running $(TARGET) with generic_charts.lua ---"
	./$(TARGET) $(EXAMPLES_DIR)/generic_charts.lua

# Run with dashboard example
run-dashboard: $(TARGET)
	@echo "--- Running $(TARGET) with dashboard.lua ---"
	./$(TARGET) $(EXAMPLES_DIR)/dashboard.lua

# Run with LCore mixed example
run-lcore: $(TARGET)
	@echo "--- Running $(TARGET) with mixed.lcore ---"
	./$(TARGET) $(EXAMPLES_DIR)/mixed.lcore

# Run with LCore report example
run-report: $(TARGET)
	@echo "--- Running $(TARGET) with report.lcore ---"
	./$(TARGET) $(EXAMPLES_DIR)/report.lcore

# Run with LCore analytics example
run-analytics: $(TARGET)
	@echo "--- Running $(TARGET) with analytics.lcore ---"
	./$(TARGET) $(EXAMPLES_DIR)/analytics.lcore

# Run with Lua DSL evaluation example
run-eval: $(TARGET)
	@echo "--- Running $(TARGET) with eval_dsl.lua ---"
	./$(TARGET) $(EXAMPLES_DIR)/eval_dsl.lua

# Run all examples in sequence
run-all: $(TARGET)
	@echo "--- Running all chart examples ---"
	@for lua_ex in $(LUA_EXAMPLES); do \
		echo "\n=== $$lua_ex ==="; \
		./$(TARGET) $$lua_ex || true; \
	done
	@for lcore_ex in $(LCORE_EXAMPLES); do \
		echo "\n=== $$lcore_ex ==="; \
		./$(TARGET) $$lcore_ex || true; \
	done

# ========== Utility Targets ==========

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)

# Clean and rebuild
rebuild: clean all

# Create examples directory if it doesn't exist
examples:
	@mkdir -p $(EXAMPLES_DIR)

# Show help
help:
	@echo "Available targets:"
	@echo "  all             - Build the executable (default)"
	@echo "  run-lua         - Run basic Lua chart example"
	@echo "  run-generic     - Run generic chart rendering example"
	@echo "  run-dashboard   - Run dashboard view example"
	@echo "  run-lcore       - Run mixed LCore example"
	@echo "  run-report      - Run LCore report example"
	@echo "  run-analytics   - Run LCore analytics example"
	@echo "  run-eval        - Run Lua DSL evaluation example"
	@echo "  run-all         - Run all examples"
	@echo "  rebuild         - Clean and rebuild"
	@echo "  clean           - Remove build artifacts"
	@echo "  help            - Show this help message"

# Phony targets
.PHONY: all run-lua run-generic run-dashboard run-lcore run-report \
        run-analytics run-eval run-all clean rebuild examples help