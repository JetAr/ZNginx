#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"

static Matrix4x4 g_proj_matrix;

static CGutModel_OpenGL g_Model_OpenGL;
static CGutModel_OpenGL g_Sphere_OpenGL;

enum {
	BUFFER_POS,
	BUFFER_NORMAL,
	BUFFER_ALBEDO,
	BUFFER_DEPTH,
	BUFFER_NUM
};

static GLuint g_Framebuffer = 0;
static GLuint g_Texture[BUFFER_NUM] = {0, 0, 0, 0};

static GLuint g_PrepareVS = 0;
static GLuint g_PreparePS = 0;
static GLuint g_PrepareProgram = 0;

static GLuint g_LightingVS = 0;
static GLuint g_LightingPS = 0;
static GLuint g_LightingProgram = 0;

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

	int w, h; GutGetWindowSize(w, h);
	float fAspect = (float)h/(float)w;
	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, fAspect, 0.1f, 100.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);

	CGutModel::SetTexturePath("../../textures/");
	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);
	g_Sphere_OpenGL.ConvertToOpenGLModel(&g_Sphere);

	GLuint fmt = GL_RGBA16F_ARB;

	if ( !GutCreateRenderTargetOpenGL(
		w, h, &g_Framebuffer, 
		fmt, g_Texture, 3,
		GL_DEPTH_COMPONENT24, &g_Texture[BUFFER_DEPTH]) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, g_Texture[BUFFER_DEPTH], 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	g_PrepareVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/DeferredLighting_Prepare.glvs");
	g_PreparePS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/DeferredLighting_Prepare.glfs");
	if ( 0==g_PrepareVS || 0==g_PreparePS )
		return false;

	g_PrepareProgram = GutCreateProgram(g_PrepareVS, g_PreparePS);
	if ( 0==g_PrepareProgram )
		return false;

	g_LightingVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/DeferredLighting_Final.glvs");
	g_LightingPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/DeferredLighting_Final.glfs");
	if ( 0==g_LightingVS || 0==g_LightingPS )
		return false;

	g_LightingProgram = GutCreateProgram(g_LightingVS, g_LightingPS);
	if ( 0==g_LightingProgram )
		return false;

	return true;
}

void DrawImage(GLuint texture, float x, float y, float w, float h, bool bInvertV = false)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(0);

	sModelMaterial_OpenGL mtl;
	mtl.m_Textures[0] = texture;
	mtl.Submit();

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

bool ReleaseResourceOpenGL(void)
{
	glDeleteFramebuffersEXT(1, &g_Framebuffer);
	for ( int i=0; i<BUFFER_NUM; i++ )
	{
		glDeleteTextures(1, &g_Texture[i]);
	}

	glDeleteShader(g_PrepareVS);
	glDeleteShader(g_PrepareVS);
	glDeleteProgram(g_PrepareProgram);

	glDeleteShader(g_LightingVS);
	glDeleteShader(g_LightingPS);
	glDeleteProgram(g_LightingProgram);

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);
}

static void DeferredLighting_Prepare(void)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffer);
	GLuint mrt[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
	glDrawBuffers(3, mrt);

	// 清除畫面
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wv_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf( (float *)&world_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *)&wv_matrix);

	glUseProgram(g_PrepareProgram);
	g_Model_OpenGL.Render(SUBMIT_TEXTURES | SUBMIT_SHADER);

	GLuint default_mrt[] = {GL_FRONT, GL_NONE, GL_NONE};
	glDrawBuffers(3, default_mrt);
}

static void DeferredLighting_PointLight(void)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 camera_matrix = g_Control.GetCameraMatrix();

	glUseProgram(g_LightingProgram);

	GLint pos_sampler = glGetUniformLocation(g_LightingProgram, "PosSampler");
	GLint normalmap_sampler = glGetUniformLocation(g_LightingProgram, "NormalmapSampler");
	GLint albedo_sampler = glGetUniformLocation(g_LightingProgram, "AlbedoSampler");

	GLint lightpos_reg = glGetUniformLocation(g_LightingProgram, "vLightPos");
	GLint lightcolor_reg = glGetUniformLocation(g_LightingProgram, "vLightColor");
	GLint lightambient_reg = glGetUniformLocation(g_LightingProgram, "vLightAmbient");
	GLint camerapos_reg = glGetUniformLocation(g_LightingProgram, "vCameraPos");

	glUniform4fv(camerapos_reg, 1, (float *)&camera_matrix[3]);

	// `Position 貼圖`
	glActiveTexture(GL_TEXTURE0_ARB);
	glUniform1i(pos_sampler, 0);
	glBindTexture(GL_TEXTURE_2D, g_Texture[BUFFER_POS]);

	// `Normalmap 貼圖`
	glActiveTexture(GL_TEXTURE1_ARB);
	glUniform1i(normalmap_sampler, 1);
	glBindTexture(GL_TEXTURE_2D, g_Texture[BUFFER_NORMAL]);

	// `Albedo 反光能力貼圖`
	glActiveTexture(GL_TEXTURE2_ARB);
	glUniform1i(albedo_sampler, 2);
	glBindTexture(GL_TEXTURE_2D, g_Texture[BUFFER_ALBEDO]);

	glActiveTexture(GL_TEXTURE0_ARB);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float*) &g_proj_matrix);

	// `不需要更新 ZBuffer`
	glDepthMask(GL_FALSE);
	// `啟動加色混色`
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_MODELVIEW);

	for ( int i=0; i<NUM_LIGHTS; i++ )
	{
		sGutLight *pLight = &g_Lights[i];

		// `更新光線資訊`
		glUniform4fv(lightpos_reg, 1, (float *)&pLight->m_vPosition);
		glUniform4fv(lightcolor_reg, 1, (float *)&pLight->m_vDiffuse);
		glUniform4fv(lightambient_reg, 1, (float *)&pLight->m_vAmbient);

		Matrix4x4 world_matrix;
		world_matrix.Scale_Replace(pLight->m_vPosition.GetWWWW());
		world_matrix[3] = pLight->m_vPosition;
		world_matrix[3][3] = 1.0f;

		Matrix4x4 wv_matrix = world_matrix * view_matrix;

		glLoadMatrixf( (float*) &wv_matrix);

		// 畫出球形
		if ( i==0 )
		{
			glDepthMask(GL_TRUE);
			g_Model_OpenGL.Render(SUBMIT_CULLFACE);
			glDepthMask(GL_FALSE);
		}
		else
		{
			g_Sphere_OpenGL.Render(SUBMIT_CULLFACE);
		}
	}

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	glUseProgram(0);
	glLoadMatrixf( (float*) &view_matrix );

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), &g_LightsVC[0].m_Position);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex_VC), &g_LightsVC[0].m_Color);
	glDrawArrays(GL_POINTS, 0, NUM_LIGHTS);
	glPopClientAttrib();

}

void RenderFrameOpenGL_(void)
{
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	DeferredLighting_Prepare();
	DeferredLighting_PointLight();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	float width = 0.4f;
	float height = 0.4f;
	float x = -1.0f;
	float y = -1.0f;

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_Texture[BUFFER_POS], x, y, width, height, true);
		x += width;
	}

	if ( g_iMode & 0x02 )
	{
		DrawImage(g_Texture[BUFFER_NORMAL], x, y, width, height, true);
		x += width;
	}

	if ( g_iMode & 0x04 )
	{
		DrawImage(g_Texture[BUFFER_ALBEDO], x, y, width, height, true);
		x += width;
	}

	GutSwapBuffersOpenGL();
}
