# Compiler settings
CC        := gcc
CFLAGS 	  := -I../../include -Wall -Wextra -O2 -fPIC
AR        := ar rcs

# Source files
SRC_UTIL  := utils/matrix.c
SRC_KAL   := Filters/LKF.c

# Build directories
BUILD_DIR := ../../build
LIB_DIR   := ../../lib
BIN_DIR   := ../../bin

# Object files
OBJ_UTIL  := $(BUILD_DIR)/matrix.o
OBJ_KAL   := $(BUILD_DIR)/LKF.o

# Final artifacts
LIB_MATRIX_A := $(LIB_DIR)/libmatrix.a
LIB_MATRIX_SO:= $(LIB_DIR)/libmatrix.so
LIB_LKF_A    := $(LIB_DIR)/liblkf.a
LIB_LKF_SO   := $(LIB_DIR)/liblkf.so
APP          := $(BIN_DIR)/lkf_app

.PHONY: all matrix liblkf clean

all: matrix liblkf

# 1) Compile each .c → .o
$(OBJ_UTIL): $(SRC_UTIL)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_KAL): $(SRC_KAL)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 2) Matrix static & shared
matrix: $(OBJ_UTIL)
	@mkdir -p $(LIB_DIR)
	$(AR) $(LIB_MATRIX_A) $^
	$(CC) -shared $^ -o $(LIB_MATRIX_SO)

# 3) LKF static & shared (includes matrix.o + LKF.o)
liblkf: $(OBJ_UTIL) $(OBJ_KAL)
	@mkdir -p $(LIB_DIR)
	$(AR) $(LIB_LKF_A) $^
	$(CC) -shared $^ -o $(LIB_LKF_SO)

# 4) Standalone app (links static matrix.a + LKF.o)
app: liblkf
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -L$(LIB_DIR) -llkf -o $(APP)

# 5) Clean up all generated files
clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR)
