#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_WIDTH 64
#define CHUNK_HEIGHT 16     

#define WATER_LEVEL CHUNK_HEIGHT / 2
#define SAND_LEVEL 3

#define BLOCK_SIZE 1

enum BlockType {
    Earth,
    Sand,
    Water,
    None,
};

struct Level {
    enum BlockType blocks[CHUNK_WIDTH][CHUNK_WIDTH];
};

struct Chunk {
    struct Level levels[CHUNK_HEIGHT];
};

void rend_block(enum BlockType block, int, int, int);
void rend_level(struct Level, int);
unsigned int rend_chunk();
void build_chunk();
enum BlockType block_at(int, int, int);
void load_textures();
int load_texture(char *);

#endif
