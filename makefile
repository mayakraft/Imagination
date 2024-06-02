INCLUDE = -I./SDL2.framework/Headers -I./SDL2_image.framework/Headers -F./
LFLAGS = -framework SDL2 -framework SDL2_image

build:
	mkdir bin/
	g++ -Wall -std=c++17 ./src/*.cpp $(INCLUDE) $(LFLAGS) -rpath ./ -o bin/imagine

run:
	./bin/imagine

clean:
	rm bin/imagine
