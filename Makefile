LIBS = -lSDL -lGL -lGLU -lm
FILES = main.c heightmap.c chunk.c
CC = gcc

mc: $(FILES)
	$(CC) $(LIBS) $(FILES)
