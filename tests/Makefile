# Compiler settings
CC        := gcc
CFLAGS 	  := -I../include -Wall -Wextra -O2 -fPIC
AR        := ar rcs

# Source files
SRC_UTIL  := ../src/Kalman/utils/matrix.c
SRC_KAL   := ../src/Kalman/Filters/LKF.c
SRC_PROC  := ../src/ImageProcessing/Processing.c
SRC_TEST_MATRIX := matrix_tests.c
SRC_TEST_KALMAN := kalman_tests.c
SRC_TEST_PROC := processing_tests.c

# Build directories
BUILD_DIR := ../build
BIN_DIR   := ../bin/tests

# Object files
OBJ_TEST_MATRIX := $(BUILD_DIR)/matrix_tests.o
OBJ_TEST_KALMAN := $(BUILD_DIR)/kalman_tests.o
OBJ_TEST_PROC := $(BUILD_DIR)/processing_tests.o
OBJ_UTIL  := $(BUILD_DIR)/matrix.o
OBJ_KAL   := $(BUILD_DIR)/LKF.o
OBJ_PROC  := $(BUILD_DIR)/Processing.o

# Final artifacts
MATRIX_TESTS := $(BIN_DIR)/matrix_tests
KALMAN_TESTS := $(BIN_DIR)/kalman_tests
PROCESSING_TESTS := $(BIN_DIR)/processing_tests
CLEAN_PATHS = $(OBJ_TEST_MATRIX) $(OBJ_TEST_KALMAN) $(OBJ_TEST_PROC) \
			  $(OBJ_UTIL) $(OBJ_KAL) $(OBJ_PROC) \
			  $(MATRIX_TESTS) $(KALMAN_TESTS) $(PROCESSING_TESTS)

.PHONY: all clean

all: matrix_tests kalman_tests processing_tests

# 1) Compile each .c → .o
$(OBJ_UTIL): $(SRC_UTIL)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_KAL): $(SRC_KAL)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PROC): $(SRC_PROC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_TEST_MATRIX): $(SRC_TEST_MATRIX)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_TEST_KALMAN): $(SRC_TEST_KALMAN)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_TEST_PROC): $(SRC_TEST_PROC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 2) Matrix tests
matrix_tests: $(OBJ_TEST_MATRIX) $(OBJ_UTIL)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(MATRIX_TESTS)

# 3) Kalman tests
kalman_tests: $(OBJ_TEST_KALMAN) $(OBJ_UTIL) $(OBJ_KAL)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(KALMAN_TESTS) -lm

# 4) Processing tests
processing_tests: $(OBJ_TEST_PROC) $(OBJ_PROC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(PROCESSING_TESTS) -lm

# 5) Clean up all generated files
clean:
	@echo "Removing: $(CLEAN_PATHS)"
	rm -f $(CLEAN_PATHS)
	rm -rf $(BUILD_DIR) $(BIN_DIR)
