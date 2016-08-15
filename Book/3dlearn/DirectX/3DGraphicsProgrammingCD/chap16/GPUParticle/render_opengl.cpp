#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"

enum
{
	TEX_POSITION0 = 0,
	TEX_VELOCITY0,
	TEX_POSITION1,
	TEX_VELOCITY1,
	TEX_TEXTURES
};

enum
{
	SHADER_SIM,
	SHADER_RENDER,
	SHADER_NUM
};

static Matrix4x4 g_proj_matrix;

static GLuint g_ShaderVS[SHADER_NUM];
static GLuint g_ShaderPS[SHADER_NUM];
static GLuint g_ShaderProgram[SHADER_NUM];

static GLuint g_Framebuffers[TEX_TEXTURES];
static GLuint g_Textures[TEX_TEXTURES];

static GLuint g_NoiseTexture = 0;
static GLuint g_ParticleTexture = 0;

static void SwapRenderTarget(int ra, int rb)
{
	GLuint framebuffer = g_Framebuffers[ra];
	GLuint texture = g_Textures[ra];

	g_Framebuffers[ra] = g_Framebuffers[rb];
	g_Textures[ra] = g_Textures[rb];

	g_Framebuffers[rb] = framebuffer;
	g_Textures[rb] = texture;
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

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	glMatrixMode(GL_MODELVIEW);	

	// glEnable(GL_DEPTH_TEST);

	GLuint fmt = GL_RGBA32F_ARB;

	if ( !GutCreateRenderTargetOpenGL(
		g_texture_width, g_texture_height, &g_Framebuffers[TEX_POSITION0], 
		fmt, &g_Textures[TEX_POSITION0], 2,
		0, NULL) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	GLuint mrt[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
	glDrawBuffers(2, mrt);
	glClear(GL_COLOR_BUFFER_BIT);

	if ( !GutCreateRenderTargetOpenGL(
		g_texture_width, g_texture_height, &g_Framebuffers[TEX_POSITION1], 
		fmt, &g_Textures[TEX_POSITION1], 2,
		0, NULL) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	GLuint default_mrt[] = {GL_FRONT, GL_NONE};
	glDrawBuffers(2, default_mrt);

	// 模擬 particle 的 shader
	g_ShaderVS[SHADER_SIM] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Transform.glvs");
	g_ShaderPS[SHADER_SIM] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/GPUParticle.glfs");
	if ( 0==g_ShaderVS[SHADER_SIM] || 0==g_ShaderPS[SHADER_SIM] )
		return false;
	g_ShaderProgram[SHADER_SIM] = GutCreateProgram(g_ShaderVS[SHADER_SIM], g_ShaderPS[SHADER_SIM]);
	if ( 0==g_ShaderProgram[SHADER_SIM] )
		return false;

	// 畫出 particle 的 shader
	g_ShaderVS[SHADER_RENDER] = GutLoadVertexShaderOpenGL_GLSL("../../shaders/GPUParticle_render.glvs");
	g_ShaderPS[SHADER_RENDER] = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/GPUParticle_render.glfs");
	if ( 0==g_ShaderVS[SHADER_RENDER] || 0==g_ShaderPS[SHADER_RENDER] )
		return false;
	g_ShaderProgram[SHADER_RENDER] = GutCreateProgram(g_ShaderVS[SHADER_RENDER], g_ShaderPS[SHADER_RENDER]);
	if ( 0==g_ShaderProgram[SHADER_RENDER] )
		return false;

	g_NoiseTexture = GutLoadTexture_OpenGL("../../textures/noise_512x512.tga");
	if ( 0==g_NoiseTexture )
		return false;

	glBindTexture(GL_TEXTURE_2D, g_NoiseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	g_ParticleTexture = GutLoadTexture_OpenGL("../../textures/particle.tga");
	if ( 0==g_ParticleTexture )
		return false;

	glBindTexture(GL_TEXTURE_2D, g_ParticleTexture);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);	

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

	glDeleteTextures(1, &g_NoiseTexture);
	glDeleteTextures(1, &g_ParticleTexture);

	for ( int i=0; i<SHADER_NUM; i++ )
	{
		glDeleteShader(g_ShaderVS[i]);
		glDeleteShader(g_ShaderPS[i]);
		glDeleteProgram(g_ShaderProgram[i]);
	}

	return true;
}

void DrawQuad(float x=-1, float y=-1, float w=2, float h=2, bool bInvertV = false)
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

void DrawImage(GLuint texture, float x=-1, float y=-1, float w=2, float h=2, bool bInvertV = false)
{
	glUseProgram(0);

	sModelMaterial_OpenGL mtl;
	mtl.m_Textures[0] = texture;
	mtl.Submit();

	DrawQuad(x, y, w, h, bInvertV);
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

static void ParticleSimulation(void)
{
	static int count = 0;

	glPushAttrib(GL_VIEWPORT_BIT);
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[TEX_POSITION1]);
	//glViewport(0, 0, g_texture_width, g_texture_height);
	//glClear(GL_COLOR_BUFFER_BIT);

	//if ( count > 2 )
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, g_texture_width, g_num_particles / g_texture_width);

		GLuint program = g_ShaderProgram[SHADER_SIM];

		glUseProgram(program);

		GLint pos_sampler = glGetUniformLocation(program, "PositionSampler");
		GLint vel_sampler = glGetUniformLocation(program, "VelocitySampler");
		GLint noise_sampler = glGetUniformLocation(program, "NoiseSampler");

		// `Position 貼圖`
		glActiveTexture(GL_TEXTURE0_ARB);
		glUniform1i(pos_sampler, 0);
		glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_POSITION0]);

		// `Velocity 貼圖`
		glActiveTexture(GL_TEXTURE1_ARB);
		glUniform1i(vel_sampler, 1);
		glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_VELOCITY0]);

		// `Noise 貼圖`
		glActiveTexture(GL_TEXTURE2_ARB);
		glUniform1i(noise_sampler, 2);
		glBindTexture(GL_TEXTURE_2D, g_NoiseTexture);

		glActiveTexture(GL_TEXTURE0_ARB);

		GLint Force_param = glGetUniformLocation(program, "Force");
		GLint LifeRange_param = glGetUniformLocation(program, "LifeRange");
		GLint SpeedRange_param = glGetUniformLocation(program, "SpeedRange");
		GLint SizeRange_param = glGetUniformLocation(program, "SizeRange");
		GLint rand_param = glGetUniformLocation(program, "Rand");
		GLint fTimeAdvance_param = glGetUniformLocation(program, "fTimeAdvance");
		GLint fTan_param = glGetUniformLocation(program, "fTan");

		Vector4 vForce = g_vForce * g_fTimeAdvance;
		Vector4 vLifeRange(1.0f, 2.0f, 0.0f, 0.0f);
		Vector4 vSpeedRange(1.0f, 2.0f, 0.0f, 0.0f);
		Vector4 vSizeRange(0.01f, 0.02f, 0.0f, 0.0f);

		Vector4 vRand[3];
		for ( int i=0; i<3; i++ )
		{
			vRand[i][0] = float(rand()%1024)/1024.0f;
			vRand[i][1] = float(rand()%1024)/1024.0f;
			vRand[i][2] = float(rand()%1024)/1024.0f;
			vRand[i][3] = float(rand()%1024)/1024.0f;
		}

		glUniform3f (Force_param, vForce[0], vForce[1], vForce[2]);
		glUniform2f (LifeRange_param, vLifeRange[0], vLifeRange[1]);
		glUniform2f (SpeedRange_param, vSpeedRange[0], vSpeedRange[1]);
		glUniform2f (SizeRange_param, vSizeRange[0], vSizeRange[1]);
		glUniform4fv(rand_param, 3, (float *)&vRand);
		glUniform1f (fTimeAdvance_param, g_fTimeAdvance);
		glUniform1f (fTan_param, FastMath::Tan(FastMath::DegToRad(g_fEmitTheta)) );

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_Framebuffers[TEX_POSITION1]);
		GLuint mrt[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
		glDrawBuffers(2, mrt);

		glViewport(0, 0, g_texture_width, g_texture_height);
		glClear(GL_COLOR_BUFFER_BIT);

		DrawQuad(-1.0f, -1.0f, 2.0f, 2.0f, true);

		GLuint default_mrt[] = {GL_FRONT, GL_NONE};
		glDrawBuffers(2, default_mrt);

		glDisable(GL_SCISSOR_TEST);
	}

	glPopAttrib();
	count++;
}

static void RenderParticels(void)
{
	int w, h; GutGetWindowSize(w, h);
	Vector4 ScreenSize( (float)w, (float)h, 0.0f, 0.0f);
	float fTanW = FastMath::Tan( FastMath::DegToRad(g_fFOV) );

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wv_matrix = world_matrix * view_matrix;

	glLoadMatrixf( (float *)&wv_matrix );

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	GLuint program = g_ShaderProgram[SHADER_RENDER];

	glUseProgram(program);

	GLint pos_sampler = glGetUniformLocation(program, "PositionSampler");
	GLint vel_sampler = glGetUniformLocation(program, "VelocitySampler");
	GLint diffuse_sampler = glGetUniformLocation(program, "diffuseSampler");

	GLint tanw_param = glGetUniformLocation(program, "fTanW");
	GLint ScreenSize_param = glGetUniformLocation(program, "ScreenSize");

	// `Position 貼圖`
	if ( pos_sampler >= 0 )
	{
		glActiveTexture(GL_TEXTURE2_ARB);
		glUniform1i(pos_sampler, 2);
		glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_POSITION1]);
	}
	// `Velocity 貼圖`
	if ( vel_sampler >=0 )
	{
		glActiveTexture(GL_TEXTURE1_ARB);
		glUniform1i(vel_sampler, 1);
		glBindTexture(GL_TEXTURE_2D, g_Textures[TEX_VELOCITY1]);
	}
	// `particle 貼圖`
	if ( diffuse_sampler >=0 )
	{
		glActiveTexture(GL_TEXTURE0_ARB);
		glUniform1i(diffuse_sampler, 0);
		glBindTexture(GL_TEXTURE_2D, g_ParticleTexture);
		//glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);	
	}

	if ( ScreenSize_param >=0 )
	{
		glUniform2fv(ScreenSize_param, 1, &ScreenSize[0]);
	}

	if ( tanw_param >=0 )
	{
		glUniform1f(tanw_param, fTanW);
	}

	int num_processed_particles = (g_num_particles / g_texture_width) * g_texture_width;

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(sParticle), g_pParticleArray);
	glDrawArrays(GL_POINTS, 0, num_processed_particles);

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glPopClientAttrib();
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	ParticleSimulation();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	RenderParticels();

	{
		float x = -1.0f;
		float y = -1.0f;
		float w = 0.3f;
		float h = 0.3f;

		if ( g_iMode & 0x01 )
		{
			DrawImage(g_Textures[TEX_POSITION1], x, y, w, h);
			x+=w;
		}

		if ( g_iMode & 0x02 )
		{
			DrawImage(g_Textures[TEX_VELOCITY1], x, y, w, h);
		}

		SwapRenderTarget(TEX_POSITION0, TEX_POSITION1);
		SwapRenderTarget(TEX_VELOCITY0, TEX_VELOCITY1);
	}

	GutSwapBuffersOpenGL();
}
