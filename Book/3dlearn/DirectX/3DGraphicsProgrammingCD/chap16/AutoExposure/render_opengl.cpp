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

struct sViewportSize
{
	int width;
	int height;
};

enum ShaderType
{
	SHADER_LUMINANCE,
	SHADER_AVERAGE,
	SHADER_ADAPTIVE,
	SHADER_AUTOEXPOSURE,
	NUM_SHADERS
};

enum FrameType
{
	FULLSIZE,
	DOWNSAMPLED_256x256,
	DOWNSAMPLED_64x64,
	DOWNSAMPLED_16x16,
	DOWNSAMPLED_4x4,
	LUMINANCE_PREVIOUS,
	LUMINANCE_CURRENT,
	LUMINANCE_TEMP,
	NUM_FRAMEBUFFERS
};

sViewportSize g_ViewportSize[] =
{
	{512,512},
	{256,256},
	{64,64},
	{16,16},
	{4,4},
	{1,1},
	{1,1},
	{1,1},
	{0,0}
};

static GLuint g_Framebuffers[NUM_FRAMEBUFFERS];
static GLuint g_Textures[NUM_FRAMEBUFFERS];
static GLuint g_depthtexture = 0;

static GLuint g_ShaderVS[NUM_SHADERS];
static GLuint g_ShaderPS[NUM_SHADERS];
static GLuint g_ShaderProgram[NUM_SHADERS];

static Matrix4x4 g_proj_matrix;
static CGutModel_OpenGL g_Model_OpenGL;

void SwapRenderTarget(int ra, int rb)
{
	GLuint framebuffer = g_Framebuffers[ra];
	GLuint texture = g_Textures[ra];

	g_Framebuffers[ra] = g_Framebuffers[rb];
	g_Textures[ra] = g_Textures[rb];

	g_Framebuffers[rb] = framebuffer;
	g_Textures[rb] = texture;
}

bool ReInitResourceOpenGL(void)
{
	int w,h;
	GutGetWindowSize(w, h);
	glViewport(0, 0, w, h);

	g_ViewportSize[0].width = w;
	g_ViewportSize[0].height = h;

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, g_fNearZ, g_fFarZ);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *)&g_proj_matrix );
	glMatrixMode(GL_MODELVIEW);

	GLuint hdr_fmt = GL_RGBA16F_ARB;

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		int w = g_ViewportSize[i].width;
		int h = g_ViewportSize[i].height;

		if ( !GutCreateRenderTargetOpenGL(
			w, h, 
			hdr_fmt, &g_Framebuffers[i], &g_Textures[i]) )
		{
			GutPushMessage("Failed to create RenderTarget");
			return false;
		}
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

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

	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);

	if ( !g_Model_OpenGL.ConvertToOpenGLModel(&g_Model) )
	{
		GutPushMessage("Failed to convert sun model");
		return false;
	}

	if ( !ReInitResourceOpenGL() )
		return false;

	// luminance shader
	g_ShaderVS[SHADER_LUMINANCE] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect.glvs");
	g_ShaderPS[SHADER_LUMINANCE] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/luminance.glfs");
	if ( 0==g_ShaderVS[SHADER_LUMINANCE] || 0==g_ShaderPS[SHADER_LUMINANCE] )
		return false;
	g_ShaderProgram[SHADER_LUMINANCE] = GutCreateProgram(g_ShaderVS[SHADER_LUMINANCE], g_ShaderPS[SHADER_LUMINANCE]);
	if ( 0==g_ShaderProgram[SHADER_LUMINANCE] )
		return false;

	// average shader
	g_ShaderVS[SHADER_AVERAGE] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect.glvs");
	g_ShaderPS[SHADER_AVERAGE] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/average.glfs");
	if ( 0==g_ShaderVS[SHADER_AVERAGE] || 0==g_ShaderPS[SHADER_AVERAGE] )
		return false;
	g_ShaderProgram[SHADER_AVERAGE] = GutCreateProgram(g_ShaderVS[SHADER_AVERAGE], g_ShaderPS[SHADER_AVERAGE]);
	if ( 0==g_ShaderProgram[SHADER_AVERAGE] )
		return false;

	// adaptive luminance shader
	g_ShaderVS[SHADER_ADAPTIVE] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect.glvs");
	g_ShaderPS[SHADER_ADAPTIVE] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/adaptive.glfs");
	if ( 0==g_ShaderVS[SHADER_ADAPTIVE] || 0==g_ShaderPS[SHADER_ADAPTIVE] )
		return false;
	g_ShaderProgram[SHADER_ADAPTIVE] = GutCreateProgram(g_ShaderVS[SHADER_ADAPTIVE], g_ShaderPS[SHADER_ADAPTIVE]);
	if ( 0==g_ShaderProgram[SHADER_ADAPTIVE] )
		return false;

	// tone mapping shader
	g_ShaderVS[SHADER_AUTOEXPOSURE] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect.glvs");
	g_ShaderPS[SHADER_AUTOEXPOSURE] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/exposure.glfs");
	if ( 0==g_ShaderVS[SHADER_AUTOEXPOSURE] || 0==g_ShaderPS[SHADER_AUTOEXPOSURE] )
		return false;
	g_ShaderProgram[SHADER_AUTOEXPOSURE] = GutCreateProgram(g_ShaderVS[SHADER_AUTOEXPOSURE], g_ShaderPS[SHADER_AUTOEXPOSURE]);
	if ( 0==g_ShaderProgram[SHADER_AUTOEXPOSURE] )
		return false;

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
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

	if ( g_depthtexture )
	{
		glDeleteTextures(1, &g_depthtexture);
		g_depthtexture = 0;
	}

	for ( int i=0; i<NUM_SHADERS; i++ )
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

void DrawImage(GLuint texture, float x, float y, float w, float h, bool bInvertV = false)
{
	glUseProgram(0);

	sModelMaterial_OpenGL mtl;
	mtl.m_Textures[0] = texture;
	mtl.Submit();

	DrawQuad(x, y, w, h, bInvertV);
}

Vector4 vLuminanceTable(0.21f, 0.71f, 0.072f);

static void ConvertToLuminance(GLuint framebuffer, GLuint texture, sViewportSize *pViewport)
{
	glViewport(0, 0, pViewport->width, pViewport->height);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	GLint LuminanceTable = glGetUniformLocation(g_ShaderProgram[SHADER_LUMINANCE], "vLuminanceTable");
	GLint ImageSampler = glGetUniformLocation(g_ShaderProgram[SHADER_LUMINANCE], "ImageSampler");

	glUseProgram(g_ShaderProgram[SHADER_LUMINANCE]);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(ImageSampler, 0);
	glUniform4fv(LuminanceTable, 1, &vLuminanceTable[0]);

	DrawFullScreenQuad(true);
}

static void AverageLuminance(GLuint texture)
{
	float fTexX, fTexY;
	Vector4 vTexOffset_256x256[16];
	Vector4 vTexOffset_64x64[16];
	Vector4 vTexOffset_16x16[16];
	Vector4 vTexOffset_4x4[16];
	int index = 0;

	index=0;
	fTexX = 1.0f/256.0f;
	fTexY = 1.0f/256.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_256x256[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/64.0f;
	fTexY = 1.0f/64.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_64x64[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/16.0f;
	fTexY = 1.0f/16.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_16x16[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/4.0f;
	fTexY = 1.0f/4.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_4x4[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	GLint ImageSampler = glGetUniformLocation(g_ShaderProgram[SHADER_AVERAGE], "ImageSampler");
	GLint TexOffset = glGetUniformLocation(g_ShaderProgram[SHADER_AVERAGE], "vTexOffset");

	// 256x256 -> 64x64
	{
		int target = DOWNSAMPLED_64x64;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[target]);
		glViewport(0, 0, g_ViewportSize[target].width, g_ViewportSize[target].height);

		glUseProgram(g_ShaderProgram[SHADER_AVERAGE]);
		glBindTexture(GL_TEXTURE_2D, g_Textures[DOWNSAMPLED_256x256]);
		glUniform1i(ImageSampler, 0);
		glUniform4fv(TexOffset, 16, (float *)vTexOffset_256x256);

		DrawFullScreenQuad(true);
	}

	// 64x64 -> 16x16
	{
		int target = DOWNSAMPLED_16x16;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[target]);
		glViewport(0, 0, g_ViewportSize[target].width, g_ViewportSize[target].height);

		glUseProgram(g_ShaderProgram[SHADER_AVERAGE]);
		glBindTexture(GL_TEXTURE_2D, g_Textures[DOWNSAMPLED_64x64]);
		glUniform1i(ImageSampler, 0);
		glUniform4fv(TexOffset, 16, (float *)vTexOffset_64x64);

		DrawFullScreenQuad(true);
	}

	// 16x16 -> 4x4
	{
		int target = DOWNSAMPLED_4x4;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[target]);
		glViewport(0, 0, g_ViewportSize[target].width, g_ViewportSize[target].height);

		glUseProgram(g_ShaderProgram[SHADER_AVERAGE]);
		glBindTexture(GL_TEXTURE_2D, g_Textures[DOWNSAMPLED_16x16]);
		glUniform1i(ImageSampler, 0);
		glUniform4fv(TexOffset, 16, (float *)vTexOffset_16x16);

		DrawFullScreenQuad(true);
	}

	// 4x4 -> 1x1
	{
		int target = LUMINANCE_CURRENT;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[target]);
		glViewport(0, 0, g_ViewportSize[target].width, g_ViewportSize[target].height);

		glUseProgram(g_ShaderProgram[SHADER_AVERAGE]);
		glBindTexture(GL_TEXTURE_2D, g_Textures[DOWNSAMPLED_4x4]);
		glUniform1i(ImageSampler, 0);
		glUniform4fv(TexOffset, 16, (float *)vTexOffset_4x4);

		DrawFullScreenQuad(true);
	}

	static bool bInitialzed = false;

	if ( !bInitialzed )
	{
		glBindTexture(GL_TEXTURE_2D, g_Textures[LUMINANCE_PREVIOUS]);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, 0, 0, 1, 1, 0);

		bInitialzed = true;
	}
}


static void AdaptiveLuminance(void)
{
	Vector4 vSpeed(0.1f);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[LUMINANCE_TEMP]);
	glViewport(0, 0, g_ViewportSize[LUMINANCE_TEMP].width, g_ViewportSize[LUMINANCE_TEMP].height);

	GLint PrevSampler = glGetUniformLocation(g_ShaderProgram[SHADER_ADAPTIVE], "PrevSampler");
	GLint CurrentSampler = glGetUniformLocation(g_ShaderProgram[SHADER_ADAPTIVE], "CurrentSampler");
	GLint AdaptiveSpeed = glGetUniformLocation(g_ShaderProgram[SHADER_ADAPTIVE], "vAdaptiveSpeed");

	glUseProgram(g_ShaderProgram[SHADER_ADAPTIVE]);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[LUMINANCE_PREVIOUS]);
	glUniform1i(PrevSampler, 0);

	glActiveTexture(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[LUMINANCE_CURRENT]);
	glUniform1i(CurrentSampler, 1);

	glActiveTexture(GL_TEXTURE0_ARB);

	glUniform4fv(AdaptiveSpeed, 1, &vSpeed[0]);

	DrawFullScreenQuad(true);

	SwapRenderTarget(LUMINANCE_PREVIOUS, LUMINANCE_TEMP);
}

static void AutoExposure(void)
{
	Vector4 vMiddle(0.5f);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, g_ViewportSize[FULLSIZE].width, g_ViewportSize[FULLSIZE].height);

	GLint HDRSampler = glGetUniformLocation(g_ShaderProgram[SHADER_AUTOEXPOSURE], "HDRSampler");
	GLint LuminanceSampler = glGetUniformLocation(g_ShaderProgram[SHADER_AUTOEXPOSURE], "LuminanceSampler");
	GLint Middle = glGetUniformLocation(g_ShaderProgram[SHADER_AUTOEXPOSURE], "vMiddle");
	GLint LuminanceTable = glGetUniformLocation(g_ShaderProgram[SHADER_AUTOEXPOSURE], "vLuminanceTable");

	glUseProgram(g_ShaderProgram[SHADER_AUTOEXPOSURE]);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[FULLSIZE]);
	glUniform1i(LuminanceSampler, 0);

	glActiveTexture(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, g_Textures[LUMINANCE_PREVIOUS]);
	glUniform1i(LuminanceSampler, 1);

	glUniform4fv(Middle, 1, &vMiddle[0]);
	glUniform4fv(LuminanceTable, 1, &vLuminanceTable[0]);

	DrawFullScreenQuad(true);
}

void RenderFrameOpenGL(void)
{
	// 清除畫面
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[FULLSIZE]);
	glViewport(0, 0, g_ViewportSize[0].width, g_ViewportSize[0].height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wv_matrix = world_matrix * view_matrix;

	glLoadMatrixf( (float *)&wv_matrix );
	g_Model_OpenGL.Render();

	ConvertToLuminance(g_Framebuffers[DOWNSAMPLED_256x256], g_Textures[FULLSIZE], &g_ViewportSize[DOWNSAMPLED_256x256]);
	AverageLuminance(g_Textures[DOWNSAMPLED_256x256]);
	AdaptiveLuminance();
	AutoExposure();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, g_ViewportSize[0].width, g_ViewportSize[0].height);

	if ( g_iMode & 0x01 )
	{
		float x = -1.0f;
		float y = -1.0f;
		float w = 0.2f;
		float h = 0.2f;

		DrawImage(g_Textures[LUMINANCE_CURRENT], x, y, w, h, true);
		x+=w;
		DrawImage(g_Textures[LUMINANCE_TEMP], x, y, w, h, true);
	}

	GutSwapBuffersOpenGL();
}
