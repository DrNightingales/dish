# Compiler and flags
CC = gcc
CFLAGS_RELEASE = -O2 -Wall -Wextra
CFLAGS_DEBUG = -g -fsanitize=address,undefined -Wall -Wextra -DDEBUG
LDFLAGS_DEBUG = -fsanitize=address,undefined
CFLAGS = $(CFLAGS_RELEASE)

# Directories
SRC_DIR = .
BUILD_DIR = build

# Sources and objects
SRC_FILES = wrappers.c main.c
OBJ_FILES = $(SRC_FILES:%.c=$(BUILD_DIR)/%.o)
DEPS = 
# Target executables
TARGET_RELEASE = shell_release
TARGET_DEBUG = shell_debug

# Default target: release version
all: $(TARGET_RELEASE)

# Release target
$(TARGET_RELEASE): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET_RELEASE) $(CFLAGS)

# Debug target
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET_DEBUG) $(CFLAGS) $(LDFLAGS_DEBUG)

# Build objects
$(BUILD_DIR)/%.o: %.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -c $< -o $@

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(TARGET_RELEASE) $(TARGET_DEBUG)

# Phony targets
.PHONY: all debug clean

