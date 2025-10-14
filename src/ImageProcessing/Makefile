# Compiler settings
CC        := gcc
CFLAGS 	  := -I../../include -Wall -Wextra -O2 -fPIC
AR        := ar rcs

# Source files
SRC_PROC  := Processing.c
SRC_PFP   := PFP.c

# Build directories
BUILD_DIR := ../../build
LIB_DIR   := ../../lib

DIRS = $(BUILD_DIR) $(LIB_DIR)

# Object files
OBJ_PROC  := $(BUILD_DIR)/Processing.o
OBJ_PFP   := $(BUILD_DIR)/PFP.o

# Final artifacts
LIB_PROC_A := $(LIB_DIR)/libProcessing.a
LIB_PROC_SO:= $(LIB_DIR)/libProcessing.so
LIB_PFP_A  := $(LIB_DIR)/libPFP.a
LIB_PFP_SO := $(LIB_DIR)/libPFP.so

CLEAN_PATHS = $(OBJ_PROC) $(LIB_PROC_A) $(LIB_PROC_SO)
CLEAN_PATHS += $(OBJ_PFP) $(LIB_PFP_A) $(LIB_PFP_SO)

.PHONY: all Processing clean

all: Processing PFP

# 1) Compile each .c → .o
$(OBJ_PROC): $(SRC_PROC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PFP): $(SRC_PFP)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 2) Processing static & shared
Processing: $(OBJ_PROC)
	@mkdir -p $(LIB_DIR)
	$(AR) $(LIB_PROC_A) $^
	$(CC) -shared $^ -o $(LIB_PROC_SO)

# PFP static & shared
PFP: $(OBJ_PFP)
	@mkdir -p $(LIB_DIR)
	$(AR) $(LIB_PFP_A) $^
	$(CC) -shared $^ -o $(LIB_PFP_SO)

# 3) Clean up all generated files
clean:
	@echo "Removing: $(CLEAN_PATHS)"
	rm -f $(CLEAN_PATHS)