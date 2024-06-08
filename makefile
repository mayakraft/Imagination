#
# building the library's empty example
#

# linux (default)
EXE = engine
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

# $(EXE): ./src/*.cpp
$(EXE): ./src/main.cpp
	mkdir -p bin/
	g++ -o bin/$@ $< $(CPPFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)

run:
	./bin/$(EXE) $(ARGS)

clean:
	rm bin/engine

#
# building the library's examples
# build with: make examples
# run with: make run1 (where 1 is any number)
#

examples: maze3d
# triangle1 triangle2 triangle3 mystify surface texture shader polyhedra glTexture

%: ./examples/%.cpp
	mkdir -p bin/
	g++ $< -o bin/$@ $(CPPFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)

run1:
	./bin/triangle1 $(ARGS)
run2:
	./bin/triangle2 $(ARGS)
run3:
	./bin/triangle3 $(ARGS)
run4:
	./bin/mystify $(ARGS)
run5:
	./bin/shader $(ARGS)
run6:
	./bin/texture $(ARGS)
run7:
	./bin/polyhedra $(ARGS)
run8:
	./bin/maze3d $(ARGS)

# unsure if it's better to write the src files as a literal
#	g++ -o bin/$@ ./src/*.cpp $(CPPFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)
