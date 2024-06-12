#
# building the library's empty example
#

# c or c++
# LANG = C++
LANG = C

# linux (default)
EXE = engine
CFLAGS = -std=c17
CPPFLAGS = -std=c++17
INCLUDE = -I./SDL2.framework/Headers \
 -I./SDL2_image.framework/Headers \
 -I./glew/include \
 -F./
LIBS = -framework SDL2 \
 -framework SDL2_image \
 -lGL \
 -lGLEW
LDFLAGS = -Wall -rpath ./

# windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	EXE = engine.exe
	# LIBS =
endif

# osx (OSTYPE not being declared)
ifndef OSTYPE
  OSTYPE = $(shell uname -s|awk '{print tolower($$0)}')
  #export OSTYPE
endif
ifeq ($(OSTYPE),darwin)
	LIBS = -framework SDL2 \
 -framework SDL2_image \
 -framework OpenGL \
 -lGLEW
endif

ifeq ($(LANG), C++)
CC = g++
FLAGS = $(CPPFLAGS)
else
CC = gcc
FLAGS = $(CFLAGS)
endif


# $(EXE): ./src/*.cpp
$(EXE): ./src/main.c
	mkdir -p bin/
	$(CC) -o bin/$@ $< $(FLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)

run:
	./bin/$(EXE) $(ARGS)

clean:
	rm bin/engine

#
# building the library's examples
# build with: make examples
# run with: make run1 (where 1 is any number)
#

examples: triangle1 triangle2 shader polyhedra mystify
# triangle3  surface texture maze3d

%: ./examples/%.c
	mkdir -p bin/
	$(CC) $< -o bin/$@ $(FLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)

run1:
	./bin/triangle1 $(ARGS)
run2:
	./bin/triangle2 $(ARGS)
run3:
	./bin/triangle3 $(ARGS)
run4:
	./bin/texture $(ARGS)
run5:
	./bin/shader $(ARGS)
run6:
	./bin/polyhedra $(ARGS)
run7:
	./bin/mystify $(ARGS)
run8:
	./bin/maze3d $(ARGS)

# unsure if it's better to write the src files as a literal
#	g++ -o bin/$@ ./src/*.cpp $(FLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)
