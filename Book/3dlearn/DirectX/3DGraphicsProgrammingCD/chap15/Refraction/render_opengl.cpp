#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutOpenGL.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static GLuint g_Texture = 0;
static Matrix4x4 g_proj_matrix;
static CGutModel_OpenGL g_Model_OpenGL;

static GLuint g_RefractionVS = 0;
static GLuint g_RefractionPS = 0;
static GLuint g_RefractionProgram = 0;

bool ReInitResourceOpenGL(void)
{
	int w,h;
	GutGetWindowSize(w, h);

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, g_fNearZ, g_fFarZ);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *)&g_proj_matrix );

	glMatrixMode(GL_MODELVIEW);

	return true;
}

// One time Init
bool InitResourceOpenGL(void)
{
	if ( NULL==glCreateProgram )
	{
		printf("Driver does not support GLSL\n");
		return false;
	}
	if ( NULL==glBindFramebufferEXT )
	{
		printf("Driver does not support framebuffer object\n");
		return false;
	}

	if ( !g_Model_OpenGL.ConvertToOpenGLModel(&g_Model) )
	{
		GutPushMessage("Failed to convert model");
		return false;
	}

	g_RefractionVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Refraction.glvs");
	g_RefractionPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/Refraction.glfs");
	if ( 0==g_RefractionVS || 0==g_RefractionPS )
		return false;
	g_RefractionProgram = GutCreateProgram(g_RefractionVS, g_RefractionPS);
	if ( 0==g_RefractionProgram )
		return false;

	g_Texture = GutLoadTexture_OpenGL("../../textures/lena.tga");

	if ( !ReInitResourceOpenGL() )
		return false;

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	glDeleteShader(g_RefractionVS);
	glDeleteShader(g_RefractionPS);
	glDeleteProgram(g_RefractionProgram);

	return true;
}

void ResizeWindowOpenGL(int width, int height)
{
	ReInitResourceOpenGL();
}

static void DrawQuad(float x, float y, float w, float h, bool bInvertV = false)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	float TopV = bInvertV ? 1.0f : 0.0f;
	float BottomV = bInvertV ? 0.0f : 1.0f;

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, BottomV);
	glVertex3f(x, y, 0.0f); 

	glTexCoord2f(1.0f, BottomV);
	glVertex3f(x+w, y, 0.0f);

	glTexCoord2f(1.0f, TopV);
	glVertex3f(x+w, y+h, 0.0f);

	glTexCoord2f(0.0f, TopV);
	glVertex3f(x, y+h, 0.0f);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static void DrawFullScreenQuad(bool bInvertV = false)
{
	DrawQuad(-1.0f, -1.0f, 2.0f, 2.0f, bInvertV);
}

void DrawImage(GLuint texture, float x=-1, float y=-1, float w=2, float h=2, bool bInvertV = false)
{
	glUseProgram(0);

	sModelMaterial_OpenGL mtl;
	mtl.m_Textures[0] = texture;
	mtl.Submit();

	DrawQuad(x, y, w, h, bInvertV);

	mtl.m_Textures[0] = 0;
	mtl.Submit();
}

void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// 畫出背景
	glDepthMask(GL_FALSE);
	DrawImage(g_Texture);
	glDepthMask(GL_TRUE);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wv_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf( (float *)&wv_matrix );

	glUseProgram(g_RefractionProgram);

	GLint bgimage_param = glGetUniformLocation(g_RefractionProgram, "BackgroundImage");
	GLint color_param = glGetUniformLocation(g_RefractionProgram, "object_color");

	Vector4 vObjectColor(1.0f, 0.0f, 0.0f);
	glUniform4fv(color_param, 1, &vObjectColor[0]);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Texture);
	glUniform1i(bgimage_param, 0);

	glLoadMatrixf( (float *)&wv_matrix );

	g_Model_OpenGL.Render(0);

	GutSwapBuffersOpenGL();
}
