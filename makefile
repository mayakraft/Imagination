LIB_NAME := libGameEngine.a

USE_VULKAN := 0

SRC_DIR := ./src
LIB_DIR := ./lib
BIN_DIR := ./bin
BUILD_DIR := ./build
INCLUDE_DIR := ./include
EXAMPLES_DIR := ./examples
FRAMEWORK_PATH := ./Frameworks
R_PATH := @executable_path/../Frameworks
VULKAN_SDK_PATH = /Library/VulkanSDK/1.3.268.1/macOS
# GLSLC = /Library/VulkanSDK/1.3.268.1/macOS/bin/glslc

CC := gcc
CXX := g++
ARCH := -arch arm64 -arch x86_64
CFLAGS := -std=c17 -Wall -Wextra -Os $(ARCH)
CXXFLAGS := -std=c++17 -Wall -Wextra -Os $(ARCH)
INCLUDE := -I$(FRAMEWORK_PATH)SDL2.framework/Headers \
 -I$(FRAMEWORK_PATH)SDL2_image.framework/Headers \
 -F$(FRAMEWORK_PATH)
LIBS := -L$(LIB_DIR) \
 -lGameEngine \
 -framework SDL2 \
 -framework SDL2_image \
 -rpath $(R_PATH)

# MacOS uses a different name for OpenGL
ifndef OSTYPE
	OSTYPE = $(shell uname -s|awk '{print tolower($$0)}')
endif
ifeq ($(OSTYPE),darwin)
	LIBS += -framework OpenGL -framework CoreFoundation
else
	LIBS += -lGL
endif

ifeq ($(USE_VULKAN), 1)
	INCLUDE += -I$(VULKAN_SDK_PATH)/include
	LIBS += -L$(VULKAN_SDK_PATH)/lib -lvulkan
endif

# Find all .c files in the source directory and subdirectories
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# Define the corresponding .o files in the build directory, mirroring the src directory structure
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Find all example files
EXAMPLE_C_FILES := $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_CPP_FILES := $(wildcard $(EXAMPLES_DIR)/*.cpp)
EXAMPLE_EXECUTABLES := $(patsubst $(EXAMPLES_DIR)/%.c, $(BIN_DIR)/%, $(EXAMPLE_C_FILES)) \
 $(patsubst $(EXAMPLES_DIR)/%.cpp, $(BIN_DIR)/%, $(EXAMPLE_CPP_FILES))

# default target
all: $(LIB_DIR)/$(LIB_NAME)

# Compile .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(INCLUDE_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Ensure include headers are copied, preserving directory structure
$(INCLUDE_DIR):
	@mkdir -p $(INCLUDE_DIR)
	cd $(SRC_DIR) && find . -name '*.h' | cpio -pdm $(abspath $(INCLUDE_DIR))

# create static library from .o files
$(LIB_DIR)/$(LIB_NAME): $(OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $(OBJ_FILES)
	strip -S $@

# build examples
examples: $(EXAMPLE_EXECUTABLES)

# .c examples
$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.c $(LIB_DIR)/$(LIB_NAME)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@ $(LIBS)

# .cpp examples
$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.cpp $(LIB_DIR)/$(LIB_NAME)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@ $(LIBS)

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR) $(INCLUDE_DIR)

.PHONY: all clean examples $(INCLUDE_DIR)
