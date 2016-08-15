#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static CGutModel_OpenGL g_Models_OpenGL[4];

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

static GLuint g_texture = 0;
static GLuint g_depthtexture = 0;
static GLuint g_framebuffer = 0;
static GLuint g_depthbuffer = 0;

static GLuint g_framebuffers[6];
static GLuint g_textures[6];

bool InitResourceOpenGL(void)
{
	if ( glGenFramebuffersEXT==NULL )
	{
		printf("Could not create frame buffer object\n");
		return false;
	}

	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `投影矩陣`
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// `設定視角轉換矩陣`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	glGenTextures(1, &g_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_texture);
	// `設定`filter
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint cubemap_id[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};


	// `配置zbuffer給framebuffer object使用`
	glGenRenderbuffersEXT(1, &g_depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 512, 512);

	int fail = 0;
	for ( int i=0; i<6; i++ )
	{
		glGenFramebuffersEXT(1, &g_framebuffers[i]);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffers[i]);
		// `宣告貼圖大小及格式`
		GLuint tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i;
		glTexImage2D(tex_target, 0, GL_RGBA8,  512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// `framebuffer的RGBA繪圖`
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, tex_target, g_texture, 0);
		// `6個不同的面可以共用同一個depthbuffer`
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthbuffer);

		// `檢查framebuffer object有沒有配置成功`
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
		{
			fail++;
		}
	}

	// `檢查framebuffer object有沒有配置成功`
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CGutModel::SetTexturePath("../../textures/");

	for ( int i=0; i<4; i++ )
	{
		g_Models_OpenGL[i].ConvertToOpenGLModel(&g_Models[i]);
	}

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_framebuffer )
	{
		glDeleteFramebuffersEXT(1, &g_framebuffer);
		g_framebuffer = 0;
	}

	if ( g_depthbuffer )
	{
		glDeleteRenderbuffersEXT(1, &g_depthbuffer);
		g_depthbuffer = 0;
	}

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

static void RenderSolarSystemOpenGL(Matrix4x4 &view_matrix)
{
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	Matrix4x4 world_view_matrix;
	// 地球
	world_view_matrix = g_earth_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[1].Render();
	// 月球
	world_view_matrix = g_moon_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[2].Render();
	// 火星
	world_view_matrix = g_mars_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[3].Render();
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// `更新動態貼圖`
	{
		Matrix4x4 cubemap_perspective_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *)&cubemap_perspective_matrix);

		// `更新cubemap的上下左右前後共6個面`
		for ( int i=0; i<6; i++ )
		{
			// `使用`g_framebuffer framebuffer object	
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffers[i]);
			glViewport(0, 0, 512, 512);
			// `清除畫面`
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// `鏡頭要面對不同的方向`
			Matrix4x4 cubemap_view = GutMatrixLookAtRH(g_vCubemap_Eye, g_vCubemap_Lookat[i], -g_vCubemap_Up[i]);
			// `畫出行星`
			RenderSolarSystemOpenGL(cubemap_view);
		}
	}
	// `使用動態貼圖`
	{
		// `使用主framebuffer object, 也就是視窗.`
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		int w, h;
		GutGetWindowSize(w, h);
		glViewport(0, 0, w, h);
		// `清除畫面`
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// `把自動計算出來的反射向量轉換到世界座標系`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 inv_view = g_Control.GetCameraMatrix();
		// `只保留矩陣的旋轉部份`
		inv_view.NoTranslate();
		// `設定貼圖轉換矩陣`
		glActiveTexture(GL_TEXTURE1_ARB);
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf((float *)&inv_view);
		// `投影矩陣`
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);
		// `轉換矩陣`
		glMatrixMode(GL_MODELVIEW);
		Matrix4x4 world_view_matrix = g_sun_matrix * view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);
		// `把Model的第3層貼圖強迫改成cubemap`
		CGutModel_OpenGL::SetTextureOverwrite(2, g_texture);
		CGutModel_OpenGL::SetMapOverwrite(2, MAP_CUBEMAP);
		//` 畫太陽`
		g_Models_OpenGL[0].Render();
		// `把Model的第3層貼圖還原成模型原始設定`
		CGutModel_OpenGL::SetTextureOverwrite(2, 0);
		CGutModel_OpenGL::SetMapOverwrite(2, MAP_NOOVERWRITE);
		// `畫其它行星`
		RenderSolarSystemOpenGL(view_matrix);
	}

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
