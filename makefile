ifndef OSTYPE
	OSTYPE = $(shell uname -s|awk '{print tolower($$0)}')
endif

EXE := engine
LIB_NAME := libGameEngine.a

SRC_DIR := ./src
LIB_DIR := ./lib
BIN_DIR := ./bin
BUILD_DIR := ./build
INCLUDE_DIR := ./include
EXAMPLES_DIR := ./examples

CC := gcc
CXX := g++
CFLAGS := -std=c17 -Wall -Wextra -O2
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

INCLUDE := -I./SDL2.framework/Headers \
 -I./SDL2_image.framework/Headers \
 -I./glew/include \
 -F./

LIBS := -L$(LIB_DIR) \
 -lGameEngine \
 -framework SDL2 \
 -framework SDL2_image \
 -lGLEW \
 -rpath ./

# MacOS uses a different name for OpenGL
ifeq ($(OSTYPE),darwin)
	LIBS += -framework OpenGL
else
	LIBS += -lGL
endif

# Find all .c files in the source directory
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Define the corresponding .o files in the build directory
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
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Ensure include headers are copied
$(INCLUDE_DIR):
	@mkdir -p $(INCLUDE_DIR)
	find $(SRC_DIR) -name '*.h' -exec cp -prv '{}' $(INCLUDE_DIR) ';'

# create static library from .o files
$(LIB_DIR)/$(LIB_NAME): $(OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $(OBJ_FILES)

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

runtriangle: $(BIN_DIR)/triangleGL
	$(BIN_DIR)/triangleGL $(ARGS)

runtriangle2: $(BIN_DIR)/triangleGLSL
	$(BIN_DIR)/triangleGLSL $(ARGS)

runmystify: $(BIN_DIR)/mystify
	$(BIN_DIR)/mystify $(ARGS)

runmaze: $(BIN_DIR)/maze3d
	$(BIN_DIR)/maze3d $(ARGS)

runshader: $(BIN_DIR)/shader
	$(BIN_DIR)/shader $(ARGS)

runpolyhedra: $(BIN_DIR)/polyhedra
	$(BIN_DIR)/polyhedra $(ARGS)

.PHONY: all clean examples $(INCLUDE_DIR)
