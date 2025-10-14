# Compiler settings
CC        := gcc
CFLAGS 	  := -I../../include -Wall -Wextra -O2 -fPIC
AR        := ar rcs

# Source files
SRC_PROC  := Processing.c

# Build directories
BUILD_DIR := ../../build
LIB_DIR   := ../../lib

DIRS = $(BUILD_DIR) $(LIB_DIR)

# Object files
OBJ_PROC  := $(BUILD_DIR)/Processing.o

# Final artifacts
LIB_PROC_A := $(LIB_DIR)/libProcessing.a
LIB_PROC_SO:= $(LIB_DIR)/libProcessing.so

CLEAN_PATHS = $(OBJ_PROC) $(LIB_PROC_A) $(LIB_PROC_SO)

.PHONY: all Processing clean

all: Processing

# 1) Compile each .c → .o
$(OBJ_PROC): $(SRC_PROC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 2) Processing static & shared
Processing: $(OBJ_PROC)
	@mkdir -p $(LIB_DIR)
	$(AR) $(LIB_PROC_A) $^
	$(CC) -shared $^ -o $(LIB_PROC_SO)

# 3) Clean up all generated files
clean:
	@echo "Removing: $(CLEAN_PATHS)"
	rm -f $(CLEAN_PATHS)