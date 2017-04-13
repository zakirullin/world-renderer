LIBS = -lSDL -lGL -lGLU -lm
FILES = src/main.c src/heightmap.c src/chunk.c
CC = gcc
OUT = -orender

mc: $(FILES)
	$(CC) $(FILES) $(LIBS) $(OUT)
