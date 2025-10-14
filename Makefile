# Root Makefile

# Path to the tests makefile
TESTS_DIR := tests
TESTS_MAKE := $(TESTS_DIR)/Makefile

# Declare phony targets
.PHONY: all clean matrix_tests kalman_tests processing_tests particle_tests

# Default target: build everything
all:
	@$(MAKE) -C $(TESTS_DIR) all

# Individual test targets
matrix_tests:
	@$(MAKE) -C $(TESTS_DIR) matrix_tests

kalman_tests:
	@$(MAKE) -C $(TESTS_DIR) kalman_tests

processing_tests:
	@$(MAKE) -C $(TESTS_DIR) processing_tests

particle_tests:
	@$(MAKE) -C $(TESTS_DIR) particle_tests

# Clean target
clean:
	@$(MAKE) -C $(TESTS_DIR) clean
