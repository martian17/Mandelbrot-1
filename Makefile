.PHONY: all run clean

SOURCE := main.cpp window_context.hpp window_context.cpp 
BINARY := Mandelbrot
DEPS   := glfw3 glew

CFLAGS := -Wall
#LIBS   := $(shell pkg-config --static --cflags --libs $(DEPS))
LIBS   := $(shell pkg-config --cflags --libs $(DEPS))

all: $(BINARY)

$(BINARY): $(SOURCE)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)

run:
	./$(BINARY)

clean:
	rm -f ./$(BINARY)