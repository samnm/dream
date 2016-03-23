
CC = cc
EXEC = dream
CFLAGS = -Wall -Wno-missing-braces
FRAMEWORKS = -framework Cocoa -framework OpenGL -framework IOKit
LIBS = -lglfw3 -lepoxy
SOURCES=$(wildcard src/*.c)
OBJS=$(SOURCES:.cpp=.o)

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

all:
	$(CC) -o bin/$(EXEC) $(OBJS) $(CFLAGS) $(LIBS) $(FRAMEWORKS) -Iinclude

clean:
	rm -f bin/$(EXEC)

run: all
	./bin/$(EXEC)
