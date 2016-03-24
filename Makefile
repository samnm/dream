
CC = cc
EXEC = dream
CFLAGS = -Wall -Wno-missing-braces -DGLFW_INCLUDE_GLCOREARB
FRAMEWORKS = -framework Cocoa -framework OpenGL -framework IOKit
LIBS = -lglfw3
SOURCES=$(wildcard src/*.c)
OBJS=$(SOURCES:.cpp=.o)

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

all:
	python scripts/make_shaders.py
	$(CC) -o bin/$(EXEC) $(OBJS) $(CFLAGS) $(LIBS) $(FRAMEWORKS) -Iinclude

clean:
	rm -rf bin/$(EXEC)

run: all
	./bin/$(EXEC)
