#ifndef HEIGHTMAP_H
#define HEIGHTSMAP_H

#include "chunk.h"

#define MAP_SIZE CHUNK_WIDTH + 1
#define ROUGH 0.9 

struct SDL_PixelFormat;

int heightmap[MAP_SIZE][MAP_SIZE];

void gen_heightmap();
void save_heightmap(struct SDL_PixelFormat*, char *);

void step_square(int, int, int, int);
void gen_water();
void normalize();
int average(int, int);
int randomize(int, int);

#endif