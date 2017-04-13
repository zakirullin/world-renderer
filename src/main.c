#include <stdio.h>
#include <time.h>
#include <math.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "chunk.h"
#include "heightmap.h"

#define TRUE 1
#define BOOL int
#define PIOVER180 0.0174532925

#define WIDTH 640
#define HEIGHT 480
#define BPP 32

#define FAR_DISTANCE 100

#define ROT_SPEED 5
#define SPEED 5

int y_rot;
int x_pos;
int y_pos;
int z_pos;

void quit(int code)
{
    SDL_Quit();
    
    exit(code);
}

void handle_key(SDL_keysym keysym)
{
    switch (keysym.sym) {
        case SDLK_UP:
            y_pos -= ROT_SPEED; 
            break;
        case SDLK_DOWN:
            y_pos += ROT_SPEED;
            break;
        case SDLK_a:
            y_rot -= ROT_SPEED;
            break;
        case SDLK_d:
            y_rot += ROT_SPEED;
            break;
        case SDLK_w:
            x_pos -= (sin(y_rot * PIOVER180)) * SPEED;
            z_pos += (cos(y_rot * PIOVER180)) * SPEED;
            break;
        case SDLK_s:
            x_pos += (sin(y_rot * PIOVER180)) * SPEED;
            z_pos -= (cos(y_rot * PIOVER180)) * SPEED;
            break;
        case SDLK_ESCAPE:
            quit(0);
            break;
    }
}

void process_events()
{
    SDL_Event event;

    // While at least one event present
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit(0);
                break;
            case SDL_KEYDOWN:
                handle_key(event.key.keysym);
                break;
        }
    }
}

void setup_opengl(int width, int height)
{
    glClearColor(0.5f, 0.9f, 1.0f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0.0f, 0.0f, width, height);
    gluPerspective(60.0f, (float)width / height, 0.1f, FAR_DISTANCE);   
}

void paint_opengl(GLuint display_list)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(y_rot, 0.0f, 1.0f, 0.0f);
    glTranslatef(x_pos, y_pos - WATER_LEVEL, z_pos);

    glCallList(display_list);

    SDL_GL_SwapBuffers();
}

int main()
{
    srand(time(NULL));
    
    y_rot = 0;
    x_pos = 0;
    y_pos = 0;
    z_pos = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Can't initialize video: %s\n", SDL_GetError());
        quit(1);
    }
    
    int flags = SDL_OPENGL | SDL_FULLSCREEN;
    SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, flags);
    if (!screen) {
        printf("Can't set video mode: %s\n", SDL_GetError());
        quit(1);
    }
    
    setup_opengl(WIDTH, HEIGHT);
    
    gen_heightmap();
    save_heightmap(screen->format, "map.bmp");
    
    build_chunk();
    load_textures();
    GLuint chunk_list = rend_chunk();
    
    SDL_EnableKeyRepeat(500, 100);
    
    while (TRUE) {
        process_events();
        
        paint_opengl(chunk_list);
    }

    return 0;
}