# Linux (default)

EXE = engine

CPPFLAGS = -std=c++17

INCLUDE = -I./SDL2.framework/Headers \
 -I./SDL2_image.framework/Headers \
 -I./glew/include \
 -F./ \

LIBS = -framework SDL2 \
 -framework SDL2_image \
 -lGL \
 -lGLEW

LDFLAGS = -Wall -rpath ./

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	EXE = engine.exe
	# LIBS =
endif

# OSX (OSTYPE not being declared)
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

$(EXE): ./src/*.cpp
	mkdir -p bin/
	g++ -o bin/$@ $< $(CPPFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)

run:
	./bin/$(EXE) $(ARGS)

clean:
	rm bin/engine

# examples (build: make all, run: make example1)
all: surface texture

%: ./examples/%.cpp
	mkdir -p bin/
	g++ $< -o bin/$@ $(CPPFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)

run1:
	./bin/surface $(ARGS)
run2:
	./bin/texture $(ARGS)

# unsure if it's better to write the src files as a literal
#	g++ -o bin/$@ ./src/*.cpp $(CPPFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS)
