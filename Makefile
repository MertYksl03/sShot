CC = gcc
CFLAGS = -Wall -Werror -g -std=c99 $(shell pkg-config --cflags gio-2.0) -Wno-maybe-uninitialized
LIBS = -lSDL3 -lSDL3_image -lm $(shell pkg-config --libs gio-2.0)
SRC_DIR = src
OBJ_DIR = obj
OUT = out

# 1. Recursively identify all source files
SOURCES = $(shell find $(SRC_DIR) -name '*.c')

# 2. Define object files, preserving directory structure (e.g., src/foo/bar.c -> obj/foo/bar.o)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Final Linking Step
$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compilation Step (Incremental)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

build: $(OUT)

run: build
	./$(OUT)

clean:
	rm -rf $(OBJ_DIR) $(OUT)

.PHONY: build run clean