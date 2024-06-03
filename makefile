# Linux (default)
EXE = imagine
CPPFLAGS = -std=c++17
INCLUDE = -I./SDL2.framework/Headers -I./SDL2_image.framework/Headers -F./
LDFLAGS = -framework SDL2 -framework SDL2_image

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	EXE = imagine.exe
	LDFLAGS = -framework SDL2 -framework SDL2_image
endif

# OSX (OSTYPE not being declared)
ifndef OSTYPE
  OSTYPE = $(shell uname -s|awk '{print tolower($$0)}')
  #export OSTYPE
endif
ifeq ($(OSTYPE),darwin)
	LDFLAGS = -framework SDL2 -framework SDL2_image
endif

$(EXE): ./src/*.cpp
	mkdir -p bin/
	g++ -o bin/$@ $< -Wall $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) -rpath ./

run:
	./bin/$(EXE) $(ARGS)

clean:
	rm bin/imagine

# examples (build: make all, run: make example1)
all: simple texture

%: ./examples/%.cpp
	mkdir -p bin/
	g++ $< -o bin/$@ $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) -Wall -rpath ./

example1:
	./bin/simple $(ARGS)
example2:
	./bin/texture $(ARGS)

# unsure if it's better to write the src files as a literal
#	g++ -o bin/$@ ./src/*.cpp -Wall $(CPPFLAGS) $(INCLUDE) $(LDFLAGS) -rpath ./
