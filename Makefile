LIBS = -lSDL -lGL -lGLU -lm
FILES = main.c heightmap.c chunk.c
CC = gcc
OUT = -orender

mc: $(FILES)
	$(CC) $(FILES) $(LIBS) $(OUT)
