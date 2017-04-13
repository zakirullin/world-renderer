#include <SDL/SDL_opengl.h>
#include <SDL/SDL_video.h>

#include "chunk.h"
#include "heightmap.h"

#define EARTH_TOP 3
#define EARTH_FRONT 4
#define SAND_TOP 5
#define SAND_FRONT 6

#define FRONT 1 << 0
#define BACK 1 << 1
#define LEFT 1 << 2
#define RIGHT 1 << 3
#define TOP 1 << 4
#define BOTTOM 1 << 5

struct Chunk chunk;

#define TEXTURES_SIZE 7
int textures[TEXTURES_SIZE];

#define COVERED_SIZE 8
int covered[COVERED_SIZE];

void rend_block(enum BlockType block, int x, int y, int z)
{
	if (block == None) {
		return;
	}
	
	glBindTexture(GL_TEXTURE_2D, textures[block]);
	
	glBegin(GL_QUADS);
		// Back side
		glTexCoord2i(0, 1); glVertex3i(x, y, z - BLOCK_SIZE);
        glTexCoord2i(1, 1); glVertex3i(x + BLOCK_SIZE, y, z - BLOCK_SIZE);
        glTexCoord2i(1, 0); glVertex3i(x + BLOCK_SIZE, y + BLOCK_SIZE, z - BLOCK_SIZE);
        glTexCoord2i(0, 0); glVertex3i(x, y + BLOCK_SIZE, z - BLOCK_SIZE);

		// Bottom side
		glTexCoord2i(0, 0); glVertex3i(x + BLOCK_SIZE, y, z);
		glTexCoord2i(1, 0); glVertex3i(x, y, z);
		glTexCoord2i(1, 1); glVertex3i(x, y, z - BLOCK_SIZE);
		glTexCoord2i(0, 1); glVertex3i(x + BLOCK_SIZE, y, z - BLOCK_SIZE);
		
		// Left side
		glTexCoord2i(0, 1); glVertex3i(x, y, z);
		glTexCoord2i(1, 1); glVertex3i(x, y, z - BLOCK_SIZE);
		glTexCoord2i(1, 0); glVertex3i(x, y + BLOCK_SIZE, z - BLOCK_SIZE);
		glTexCoord2i(0, 0); glVertex3i(x, y + BLOCK_SIZE, z);
		
		// Right side
		glTexCoord2i(0, 1); glVertex3i(x + BLOCK_SIZE, y, z);
		glTexCoord2i(1, 1); glVertex3i(x + BLOCK_SIZE, y, z - BLOCK_SIZE);
		glTexCoord2i(1, 0); glVertex3i(x + BLOCK_SIZE, y + BLOCK_SIZE, z - BLOCK_SIZE);
		glTexCoord2i(0, 0); glVertex3i(x + BLOCK_SIZE, y + BLOCK_SIZE, z);
    glEnd();
	
	if (block == Earth) {
		glBindTexture(GL_TEXTURE_2D, textures[EARTH_TOP]);
	} else if (block == Sand) {
		glBindTexture(GL_TEXTURE_2D, textures[SAND_TOP]);
	}

	glBegin(GL_QUADS);
		// Top side
		glTexCoord2i(0, 0); glVertex3i(x, y + BLOCK_SIZE, z);
		glTexCoord2i(1, 0); glVertex3i(x + BLOCK_SIZE, y + BLOCK_SIZE, z);
		glTexCoord2i(1, 1); glVertex3i(x + BLOCK_SIZE, y + BLOCK_SIZE, z - BLOCK_SIZE);
		glTexCoord2i(0, 1); glVertex3i(x, y + BLOCK_SIZE, z - BLOCK_SIZE);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, textures[block]);
	
	if (block == Earth) {
		glBindTexture(GL_TEXTURE_2D, textures[EARTH_FRONT]);
	} else if (block == Sand) {
		glBindTexture(GL_TEXTURE_2D, textures[SAND_FRONT]);
	}
	
	glBegin(GL_QUADS);
		// Front side
		glTexCoord2i(0, 1); glVertex3i(x, y, z);
        glTexCoord2i(1, 1); glVertex3i(x + BLOCK_SIZE, y, z);
        glTexCoord2i(1, 0); glVertex3i(x + BLOCK_SIZE, y + BLOCK_SIZE, z);
        glTexCoord2i(0, 0); glVertex3i(x, y + BLOCK_SIZE, z);
	glEnd();
}

void rend_level(struct Level level, int y)
{
	int x;
	int z;
    for (z = 0; z < CHUNK_WIDTH; z++) {
        for (x = 0; x < CHUNK_WIDTH; x++) {
            rend_block(level.blocks[x][z], x, y, z * -1);
        }
    }
}

unsigned int rend_chunk()
{
	unsigned int chunk_list = glGenLists(1);
	glNewList(chunk_list, GL_COMPILE);

	int i = 0;
	for (i = 0; i < CHUNK_HEIGHT; i++) {
        rend_level(chunk.levels[i], i);
    }
	
	glEndList();
	
	return chunk_list;
}

void build_chunk()
{
	
	printf("%x", FRONT);
	int x;
	int y;
	int z;
	for (y = 0; y < CHUNK_HEIGHT; y++) {
		for (z = 0; z < CHUNK_WIDTH; z++) {
			for (x = 0; x < CHUNK_WIDTH; x++) {
				chunk.levels[y].blocks[x][z] = block_at(x, y, z);
			}
		}
	}


	// Mark invisble blocks as Covered 
	for (y = 0; y < CHUNK_HEIGHT; y++) {
		for (z = 0; z < CHUNK_WIDTH; z++) {
			for (x = 0; x < CHUNK_WIDTH; x++) {
				if (chunk.levels[y].blocks[x][z] == None) {
					break;
				}
			
				int is_left = block_at(x - 1, y, z) != None;
				int is_right = block_at(x + 1, y, z) != None;
				int is_top = block_at(x, y + 1, z) != None;
				int is_bottom = block_at(x, y - 1, z) != None;
				int is_front = block_at(x, y, z - 1) != None;
				int is_back = block_at(x, y, z + 1) != None;
			}
		}
	}
}

enum BlockType block_at(int x, int y, int z)
{
	if (x == -1 || y == -1 || z == -1) {
		return None;
	}
	
	enum BlockType block;
	int max_height = heightmap[x][z];
	
	if (y <= max_height) {
		if (y < WATER_LEVEL) {
			block = None;
		} else if (max_height == WATER_LEVEL) {
			block = Water;
		} else if (max_height <= WATER_LEVEL + SAND_LEVEL) {
			block = Sand;
		} else {
			block = Earth;
		}
	} else {
		block = None;
	}
	
	return block;
}

void load_textures()
{
	textures[Earth] = load_texture("earth.bmp");
	textures[Sand] = load_texture("sand.bmp");
	textures[Water] = load_texture("water.bmp");
	textures[EARTH_TOP] = load_texture("earth_top.bmp");
	textures[EARTH_FRONT] = load_texture("earth_front.bmp");
	textures[SAND_TOP] = load_texture("sand_top.bmp");
	textures[SAND_FRONT] = load_texture("sand_front.bmp");
}

int load_texture (char *filename)
{
	SDL_Surface *tex = SDL_LoadBMP(filename);

    int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex->w, tex->h, 0, GL_BGR, GL_UNSIGNED_BYTE, tex->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(tex);

    return texture;
}
