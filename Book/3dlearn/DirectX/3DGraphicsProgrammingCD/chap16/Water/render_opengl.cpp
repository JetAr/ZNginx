#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"

enum BufferType
{
	TEX_HEIGHT0,
	TEX_HEIGHT1,
	TEX_HEIGHT2,
	TEX_NORMAL,
	TEX_TEXTURES
};

enum ShaderType
{
	SHADER_ADDIMPULSE,
	SHADER_WATERSIM,
	SHADER_TONORMAL,
	SHADER_WATER,
	SHADER_SHADERS
};

static GLuint g_Framebuffers[TEX_TEXTURES];
static GLuint g_Textures[TEX_TEXTURES];
static GLuint g_WaterTexture = 0;

static GLuint g_ShaderVS[SHADER_SHADERS];
static GLuint g_ShaderPS[SHADER_SHADERS];
static GLuint g_ShaderProgram[SHADER_SHADERS];

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_orient_matrix;

static CGutModel_OpenGL g_Model_OpenGL;

bool ReInitResourceOpenGL(void)
{
	GutGetWindowSize(g_iWidth, g_iHeight);
	// 投影矩陣
	g_proj_matrix = GutMatrixOrthoRH_OpenGL(g_iWidth, g_iHeight, 1.0f, 100.0f);
	//g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, 1.0f, 0.1f, 1000.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	glMatrixMode(GL_MODELVIEW);	

	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		if ( !GutCreateRenderTargetOpenGL(g_iWidth, g_iHeight, GL_RGBA, &g_Framebuffers[i], &g_Textures[i]) )
			return false;
	}

	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

	return true;
}

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

	g_ShaderVS[SHADER_ADDIMPULSE] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Transform.glvs");
	g_ShaderPS[SHADER_ADDIMPULSE] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/AddImpulse.glfs");
	if ( 0==g_ShaderVS[SHADER_ADDIMPULSE] || 0==g_ShaderPS[SHADER_ADDIMPULSE] )
		return false;
	g_ShaderProgram[SHADER_ADDIMPULSE] = GutCreateProgram(g_ShaderVS[SHADER_ADDIMPULSE], g_ShaderPS[SHADER_ADDIMPULSE]);

	g_ShaderVS[SHADER_WATERSIM] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Transform.glvs");
	g_ShaderPS[SHADER_WATERSIM] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/WaterSim.glfs");
	if ( 0==g_ShaderVS[SHADER_WATERSIM] || 0==g_ShaderPS[SHADER_WATERSIM] )
		return false;
	g_ShaderProgram[SHADER_WATERSIM] = GutCreateProgram(g_ShaderVS[SHADER_WATERSIM], g_ShaderPS[SHADER_WATERSIM]);

	g_ShaderVS[SHADER_TONORMAL] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Transform.glvs");
	g_ShaderPS[SHADER_TONORMAL] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/HeightToNormal.glfs");
	if ( 0==g_ShaderVS[SHADER_TONORMAL] || 0==g_ShaderPS[SHADER_TONORMAL] )
		return false;
	g_ShaderProgram[SHADER_TONORMAL] = GutCreateProgram(g_ShaderVS[SHADER_TONORMAL], g_ShaderPS[SHADER_TONORMAL]);

	g_ShaderVS[SHADER_WATER] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Transform.glvs");
	g_ShaderPS[SHADER_WATER] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/Water.glfs");
	if ( 0==g_ShaderVS[SHADER_WATER] || 0==g_ShaderPS[SHADER_WATER] )
		return false;
	g_ShaderProgram[SHADER_WATER] = GutCreateProgram(g_ShaderVS[SHADER_WATER], g_ShaderPS[SHADER_WATER]);

	g_WaterTexture = GutLoadTexture_OpenGL("../../textures/nasa_stars.tga");
	if ( 0==g_WaterTexture )
		return false;

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	if ( !ReInitResourceOpenGL() )
		return false;

	g_orient_matrix.Identity();

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		if ( g_Framebuffers[i] )
		{
			glDeleteFramebuffersEXT(1, &g_Framebuffers[i]);
			g_Framebuffers[i] = 0;
		}
		if ( g_Textures[i] )
		{
			glDeleteTextures(1, &g_Textures[i]);
			g_Textures[i] = 0;
		}
	}

	glDeleteTextures(1, &g_WaterTexture);

	for ( int i=0; i<SHADER_SHADERS; i++ )
	{
		glDeleteShader(g_ShaderVS[i]);
		glDeleteShader(g_ShaderPS[i]);
		glDeleteProgram(g_ShaderProgram[i]);
	}

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	//
	ReInitResourceOpenGL();
}

void SwapRenderTarget(int ra, int rb)
{
	GLuint framebuffer = g_Framebuffers[ra];
	GLuint texture = g_Textures[ra];

	g_Framebuffers[ra] = g_Framebuffers[rb];
	g_Textures[ra] = g_Textures[rb];

	g_Framebuffers[rb] = framebuffer;
	g_Textures[rb] = texture;
}

static void SwapHeightmaps(void)
{
	// 0 -> 1 -> 2 to 2 -> 0 ->1

	GLuint framebuffer = g_Framebuffers[TEX_HEIGHT0];
	GLuint texture = g_Textures[TEX_HEIGHT0];

	g_Framebuffers[TEX_HEIGHT0] = g_Framebuffers[TEX_HEIGHT1];
	g_Textures[TEX_HEIGHT0] = g_Textures[TEX_HEIGHT1];

	g_Framebuffers[TEX_HEIGHT1] = g_Framebuffers[TEX_HEIGHT2];
	g_Textures[TEX_HEIGHT1] = g_Textures[TEX_HEIGHT2];

	g_Framebuffers[TEX_HEIGHT2] = framebuffer;
	g_Textures[TEX_HEIGHT2] = texture;
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

void DrawImage(GLuint texture, float x, float y, float w, float h, bool bInvertV = false)
{
	glUseProgram(0);

	sModelMaterial_OpenGL mtl;
	mtl.m_Textures[0] = texture;
	mtl.Submit();

	DrawQuad(x, y, w, h, bInvertV);

	mtl.m_Textures[0] = 0;
	mtl.Submit();
}

static void AddImpulse(void)
{
	static Vector4 vPosition(0.0f, 0.0f, 0.0f, 0.0f);
	//Vector4 vDiff = vPosition - g_vPosition;
	Vector4 vDiff = g_vPosition - vPosition;
	Vector4 vLength = vDiff.Length();

	if ( vLength[0]<2.0f )
		return;

	Vector4 vDir = vDiff / vLength;
	Vector4 vVec0(vDir[1],-vDir[0], 0.0f, 0.0f);
	Vector4 vVec1(vDir[0], vDir[1], 0.0f, 0.0f);

	vPosition = g_vPosition;

	Vector4 vVec0_old = g_orient_matrix[0];
	Vector4 vVec1_old = g_orient_matrix[1];

	Vector4 vVec0_new = VectorLerp(vVec0_old, vVec0, 0.2f);
	Vector4 vVec1_new = VectorLerp(vVec1_old, vVec1, 0.2f);
	vVec0_new.Normalize();
	vVec1_new.Normalize();
	Vector4 vVec2_new = Vector3CrossProduct(vVec0_new, vVec1_new);

	g_orient_matrix.Identity();
	g_orient_matrix[0] = vVec0_new;
	g_orient_matrix[1] = vVec1_new;
	g_orient_matrix[2] = vVec2_new;

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();

	Matrix4x4 scale_matrix; 
	scale_matrix.Scale_Replace(g_fRippleSize, g_fRippleSize, 1.0f);

	Matrix4x4 world_matrix = g_orient_matrix * scale_matrix;
	world_matrix[3] = g_vPosition;

	Matrix4x4 vp_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *)&vp_matrix );

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[TEX_HEIGHT1]);

	Vector4 vForce(0.1f);

	GLint force_var = glGetUniformLocation(g_ShaderProgram[SHADER_ADDIMPULSE], "vForce");

	glUseProgram(g_ShaderProgram[SHADER_ADDIMPULSE]);
	glUniform4fv(force_var, 1, &vForce[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	g_Model_OpenGL.Render(0);

	glDisable(GL_BLEND);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

static void WaterSimulation(void)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[TEX_HEIGHT2]);

	GLuint program = g_ShaderProgram[SHADER_WATERSIM];

	GLint heightmap_current_param = glGetUniformLocation(program, "HeightCurrentSampler");
	GLint heightmap_prev_param = glGetUniformLocation(program, "HeightPrevSampler");
	GLint texturesize_param = glGetUniformLocation(program, "vTextureSize");
	GLint damping_param = glGetUniformLocation(program, "fDamping");

	Vector4 texturesize;

	texturesize[0] = 1.0f/(float)g_iWidth;
	texturesize[1] = 1.0f/(float)g_iHeight;
	texturesize[2] = (float)g_iWidth;
	texturesize[3] = (float)g_iHeight;

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_HEIGHT0]);
	glUniform1i(heightmap_prev_param, 0);

	glActiveTexture(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_HEIGHT1]);
	glUniform1i(heightmap_current_param, 1);

	glUniform4fv(texturesize_param, 1, &texturesize[0]);
	glUniform1f(damping_param, 0.9f);

	DrawFullScreenQuad(true);
}

static void HeightmapToNormal(void)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[TEX_NORMAL]);

	GLuint program = g_ShaderProgram[SHADER_TONORMAL];

	GLint heightmap_current_param = glGetUniformLocation(program, "HeightCurrentSampler");
	GLint texturesize_param = glGetUniformLocation(program, "vTextureSize");
	GLint normalscale_param = glGetUniformLocation(program, "fNormalScale");

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_HEIGHT2]);
	glUniform1i(heightmap_current_param, 0);

	Vector4 texturesize;

	texturesize[0] = 1.0f/(float)g_iWidth;
	texturesize[1] = 1.0f/(float)g_iHeight;
	texturesize[2] = (float)g_iWidth;
	texturesize[3] = (float)g_iHeight;

	glUniform4fv(texturesize_param, 1, &texturesize[0]);
	glUniform1f(normalscale_param, 1.0f);

	DrawFullScreenQuad(true);
}

static void RenderWater(void)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	GLuint program = g_ShaderProgram[SHADER_WATER];

	GLint normalmap_param = glGetUniformLocation(program, "NormalmapSampler");
	GLint watertex_param = glGetUniformLocation(program, "WaterSampler");
	GLint texcoordscale_param = glGetUniformLocation(program, "fTexcoordScale");

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_NORMAL]);
	glUniform1i(normalmap_param, 1);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_WaterTexture);
	glUniform1i(watertex_param, 0);

	glUniform1f(texcoordscale_param, 0.2f);

	DrawFullScreenQuad(true);
}

static void DrawObject(void)
{
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();

	Matrix4x4 scale_matrix; 
	scale_matrix.Scale_Replace(g_fRippleSize, g_fRippleSize, 1.0f);

	Matrix4x4 world_matrix = g_orient_matrix * scale_matrix;
	world_matrix[3] = g_vPosition;

	Matrix4x4 wv_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *)&wv_matrix );

	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	g_Model_OpenGL.Render();
	glDisable(GL_DEPTH_TEST);

}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	AddImpulse();
	WaterSimulation();
	HeightmapToNormal();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	RenderWater();
	DrawObject();
	SwapHeightmaps();

	float x= -1.0f;
	float y= -1.0f;
	float w = 0.3f;
	float h = 0.3f;

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_Textures[TEX_HEIGHT2], x, y, w, h, true);
		x+=w;
	}

	if ( g_iMode & 0x02 )
	{
		DrawImage(g_Textures[TEX_NORMAL], x, y, w, h, true);
		x+=w;
	}

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
