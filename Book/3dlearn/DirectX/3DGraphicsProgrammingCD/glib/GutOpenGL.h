#ifndef _GUTOPENGL_
#define _GUTOPENGL_

#include "GutDefs.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

bool GutInitGraphicsDeviceOpenGL(GutDeviceSpec *spec=NULL);
bool GutReleaseGraphicsDeviceOpenGL(void);
void GutSwapBuffersOpenGL(void);

GLuint GutLoadVertexProgramOpenGL_ASM(const char *filename);
GLuint GutLoadFragmentProgramOpenGL_ASM(const char *filename);

void GutReleaseVertexProgramOpenGL(GLuint shader_id);
void GutReleasePixelProgramOpenGL(GLuint shader_id);

GLuint GutLoadVertexShaderOpenGL_GLSL(const char *filename);
GLuint GutLoadFragmentShaderOpenGL_GLSL(const char *filename);
GLuint GutCreateProgram(GLuint vs, GLuint fs);

void GutReleaseVertexShaderGLSL(GLuint shader_id);
void GutReleaseFragmentShaderGLSL(GLuint shader_id);

bool GutCreateRenderTargetOpenGL(int w, int h, GLuint color_fmt, GLuint *framebuffer, GLuint *texture);
bool GutCreateRenderTargetOpenGL(int w, int h, GLuint *framebuffer, 
								 GLuint color_fmt, GLuint *frametexture,
								 GLuint depth_fmt, GLuint *depthtexture);
bool GutCreateRenderTargetOpenGL(int w, int h, GLuint *pFrameBuffer,
								 GLuint color_fmt, GLuint *pFrameTexture, int num_mrts,
								 GLuint depth_fmt, GLuint *pDepthTexture);

void GutSetupLightOpenGL(int index, sGutLight &light);

#endif // _GUTOPENGL_