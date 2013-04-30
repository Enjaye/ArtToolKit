#ifndef LOADTEXTURE_H
#define LOADTEXTURE_H

#include <windows.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif


SDL_Surface * flipSurface(SDL_Surface * surface);
bool charge_texture( char *nom, GLuint indice,GLuint *texture,bool mipmap );

#endif //LOADTEXTURE_H
