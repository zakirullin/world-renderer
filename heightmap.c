#include <stdio.h>
#include <SDL/SDL.h>

#include "heightmap.h"

void gen_heightmap()
{
	int i;
	int j;
	for (i = 0; i < MAP_SIZE; i++) {
		for (j = 0; j < MAP_SIZE; j++)
			heightmap[i][j] = 0;
	}
	
	// Set initial values at corners of square
	heightmap[0][0] = WATER_LEVEL + randomize(-10, 10); 
	heightmap[MAP_SIZE - 1][0] = WATER_LEVEL - randomize(-10, 10);
	heightmap[MAP_SIZE - 1][MAP_SIZE - 1] = WATER_LEVEL + randomize(-10, 10);
	heightmap[0][MAP_SIZE -1] = WATER_LEVEL - randomize(-10, 10); 
	
	step_square(0, 0, MAP_SIZE - 1, MAP_SIZE - 1);
	
	gen_water();
	normalize();
}

void save_heightmap(SDL_PixelFormat *format, char *filename)
{
	Uint32 *pixels = calloc(CHUNK_WIDTH * CHUNK_WIDTH, 4);
	int x;
	int y;
	for (y = 0; y < CHUNK_WIDTH; y++) {
		for (x = 0; x < CHUNK_WIDTH; x++) {
			Uint32 *bufp = pixels + (y * CHUNK_WIDTH) + x;
			Uint32 color;
			int height = heightmap[x][y];
			if (height <= WATER_LEVEL)
				color = SDL_MapRGB(format, 0, 0, height % 255);
			else if (height <= WATER_LEVEL + SAND_LEVEL)
				color = SDL_MapRGB(format, 255, 201, 175);
			else
				color = SDL_MapRGB(format, 0, height % 256, 0);
			*bufp = color;
		}
	}
	
	SDL_Surface *image = SDL_CreateRGBSurfaceFrom(pixels, CHUNK_WIDTH, CHUNK_WIDTH, format->BitsPerPixel, CHUNK_WIDTH * 4, format->Rmask, format->Gmask, format->Bmask, format->Amask);
	SDL_SaveBMP(image, "img.bmp");
	
	SDL_FreeSurface(image);
	
	free(pixels);
}

void step_square(int x0, int y0, int x1, int y1)
{
	// Finish recursion if needed.
	if (((x1 - x0) <= 1) || (y1 - y0) <= 1)
		return;
	
	// Middle coords.
	int mid_x = (x0 + x1) / 2;
	int mid_y = (y0 + y1) / 2;
	
	// Heights at corners.
	int top_left_height = heightmap[x0][y0];
	int top_right_height = heightmap[x1][y0];
	int bot_left_height = heightmap[x0][y1];
	int bot_right_height = heightmap[x1][y1];
	
	// Left
	heightmap[x0][mid_y] = average(top_left_height, bot_left_height);
	
	// Top
	heightmap[mid_x][y0] = average(top_left_height, top_right_height);
	
	// Right
	heightmap[x1][mid_y] = average(top_right_height, bot_right_height);
	
	// Bottom
	heightmap[mid_x][y1] = average(bot_left_height, bot_right_height);
	
	// Center point average
	heightmap[mid_x][mid_y] = (top_left_height + top_right_height + bot_left_height + bot_right_height) / 4;
	
	// Center point displace
	int length = x1 - x0;
	heightmap[mid_x][mid_y] += randomize(-1 * ROUGH * length, ROUGH * length);
	
	// Repeat for smaller squares
	step_square(x0, y0, mid_x, mid_y);
	step_square(mid_x, y0, x1, mid_y);
	step_square(mid_x, mid_y, x1, y1);
	step_square(x0, mid_y, mid_x, y1);
}

void gen_water()
{
	int i;
	int j;
	for (i = 0; i < MAP_SIZE; i++) {
		for (j = 0; j < MAP_SIZE; j++) {
			if (heightmap[i][j] < WATER_LEVEL)
				heightmap[i][j] = WATER_LEVEL;
		}
	}
}

void normalize()
{
	int i;
	int j;
	for (i = 0; i < MAP_SIZE; i++) {
		for (j = 0; j < MAP_SIZE; j++) {
			if (heightmap[i][j] >= CHUNK_HEIGHT)
				heightmap[i][j] = CHUNK_HEIGHT;
			if (heightmap[i][j] < 0)
				heightmap[i][j] = 0;
		}
	}
}

int average(int a, int b)
{
	return (a + b) / 2;
}

int randomize(int min, int max)
{
	return (rand() % (max - min)) + min;
}