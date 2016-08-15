#ifndef _GUTTEXTURE_OPENGL_
#define _GUTTEXTURE_OPENGL_

#include "GutImage.h"

GLuint GutLoadTexture_OpenGL(const char *filename, sImageInfo *pInfo=NULL, bool mipmap_enabled = true);
GLuint GutLoadCubemapTexture_OpenGL(const char **filename_array, bool mipmap_enabled = true);
GLuint GutLoadCubemapTexture_OpenGL(const char *filename);
GLuint GutLoadVolumeTexture_OpenGL(const char *filename);

#endif // _GUTTEXTURE_OPENGL_