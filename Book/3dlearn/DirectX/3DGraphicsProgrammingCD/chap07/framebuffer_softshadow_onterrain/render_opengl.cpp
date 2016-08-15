#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;

const int g_framebuffer_w = 256;
const int g_framebuffer_h = 256;

static CGutModel_OpenGL g_Model_OpenGL;
static CGutModel_OpenGL g_Terrain_OpenGL;

static GLuint g_texture = 0;
static GLuint g_depthtexture = 0;
static GLuint g_framebuffer = 0;
static GLuint g_depthbuffer = 0;

static GLuint g_blurframebuffer[2];
static GLuint g_blurtexture[2];

bool InitResourceOpenGL(void)
{
	if ( glGenFramebuffersEXT==NULL )
	{
		printf("Could not create frame buffer object\n");
		return false;
	}

	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glDisable(GL_CULL_FACE);

	// 開啟一個framebuffer object
	glGenFramebuffersEXT(1, &g_framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer);

	// 配罝一塊貼圖空間給framebuffer object繪圖使用 
	{
		glGenTextures(1, &g_texture);
		glBindTexture(GL_TEXTURE_2D, g_texture);
		// 設定filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// 宣告貼圖大小及格式
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  g_framebuffer_w, g_framebuffer_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// framebuffer的RGBA繪圖
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texture, 0);
	}

	// 配置zbuffer給framebuffer object使用
	{
		glGenRenderbuffersEXT(1, &g_depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, g_framebuffer_w, g_framebuffer_h);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthbuffer);
	}

	// 檢查framebuffer object有沒有配置成功
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	for ( int i=0; i<2; i++ )
	{
		// 再開啟一個framebuffer object
		glGenFramebuffersEXT(1, &g_blurframebuffer[i]);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_blurframebuffer[i]);

		// 配罝一塊貼圖空間給framebuffer object繪圖使用 
		{
			glGenTextures(1, &g_blurtexture[i]);
			glBindTexture(GL_TEXTURE_2D, g_blurtexture[i]);
			// 設定filter
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// 宣告貼圖大小及格式
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  g_framebuffer_w, g_framebuffer_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// framebuffer的RGBA繪圖
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_blurtexture[i], 0);
		}

		// 檢查framebuffer object有沒有配置成功
		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
		{
			return false;
		}
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);
	g_Terrain_OpenGL.ConvertToOpenGLModel(&g_Terrain);

	for ( int i=0; i<4; i++ )
	{
		g_Quad[i].m_Texcoord[1] = 1.0f - g_Quad[i].m_Texcoord[1];
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

// 7x7 blur
GLuint BlurTexture(GLuint source)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_blurframebuffer[0]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, g_framebuffer_w, g_framebuffer_h);

	sModelMaterial_OpenGL material;

	material.m_Ambient = Vector4::GetOne();
	material.m_Diffuse = Vector4::GetOne();
	material.m_Emissive = Vector4::GetOne();

	material.m_Textures[0] = source;
	material.m_bCullFace = false;
	material.m_bBlend = true;
	material.m_SrcBlend = GL_ONE;
	material.m_DestBlend = GL_ONE;
	material.Submit(NULL);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	const int table_size = 9;
	float weight_table[table_size] = {1.0f,2.0f, 3.5f, 4.5f, 5.0f, 4.5f, 3.5f, 2.0f, 1.0f};
	float uv_offset_table[table_size] = {-4.0f,-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f,4.0f};

	float sum = 0.0f;

	for ( int i=0; i<table_size; i++ )
	{
		sum += weight_table[i];
	}

	for ( int i=0; i<table_size; i++ )
	{
		weight_table[i] /= sum;
	}

	Vector4 vOneTexel_X(1.0f/(float)g_framebuffer_w, 0.0f, 0.0f, 0.0f);
	Vector4 vOneTexel_Y(0.0f, 1.0f/(float)g_framebuffer_h, 0.0f, 0.0f);
	Vertex_VT temp_quad[4];


	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// X方向blur
	for ( int i=0; i<table_size; i++ )
	{
		Vector4 vOffset = uv_offset_table[i] * vOneTexel_X;

		for ( int p=0; p<4; p++ )
		{
			temp_quad[p].m_Position = g_Quad[p].m_Position;
			temp_quad[p].m_Texcoord = g_Quad[p].m_Texcoord + vOffset;
		}

		glColor4f(weight_table[i], weight_table[i], weight_table[i], weight_table[i]);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &temp_quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &temp_quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_blurframebuffer[1]);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g_blurtexture[0]);

	// Y方向blur
	for ( int i=0; i<=table_size; i++ )
	{
		Vector4 vOffset = uv_offset_table[i] * vOneTexel_Y;

		for ( int p=0; p<4; p++ )
		{
			temp_quad[p].m_Position = g_Quad[p].m_Position;
			temp_quad[p].m_Texcoord = g_Quad[p].m_Texcoord + vOffset;
		}

		glColor4f(weight_table[i], weight_table[i], weight_table[i], weight_table[i]);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &temp_quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &temp_quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	return g_blurframebuffer[1];
}

void SetupLightingOpenGL(void)
{
	int LightID = GL_LIGHT0;

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(LightID);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &g_vAmbientLight[0]);
	glLightfv(LightID, GL_POSITION, &g_Light.m_Position[0]);
	glLightfv(LightID, GL_DIFFUSE, &g_Light.m_Diffuse[0]);
	glLightfv(LightID, GL_SPECULAR, &g_Light.m_Specular[0]);

	glLightf(LightID, GL_CONSTANT_ATTENUATION,	1.0f);
	glLightf(LightID, GL_LINEAR_ATTENUATION,	0.0f);
	glLightf(LightID, GL_QUADRATIC_ATTENUATION, 0.0f);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	Matrix4x4 light_projection_matrix;
	Matrix4x4 light_view_matrix;
	Matrix4x4 light_world_view_matrix;

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	GLuint blurred_texture = 0;

	{
		// 使用g_framebuffer framebuffer object	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer);
		glViewport(0, 0, g_framebuffer_w, g_framebuffer_h);
		// 清除畫面
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 設定光源位置
		Vector4 vLightPos = g_Light.m_Position;
		Vector4 vLightUp(0.0f, 1.0f, 0.0f);
		Vector4 vLightLookat(0.0f, 0.0f, 0.0f);

		light_projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, 1.0f, 0.1f, 100.0f);
		light_view_matrix = GutMatrixLookAtRH(vLightPos, vLightLookat, vLightUp);
		light_world_view_matrix = world_matrix * light_view_matrix;

		// 把鏡頭放到光源位置來畫陰影
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &light_projection_matrix );
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &light_world_view_matrix );

		SetupLightingOpenGL();

		// 套用預設材質, 把貼圖全部關閉
		sModelMaterial_OpenGL material;

		material.m_Ambient = Vector4::GetZero();
		material.m_Emissive = Vector4::GetZero();
		material.m_Diffuse = Vector4::GetZero();
		material.m_Specular = Vector4::GetZero();

		material.m_bCullFace = false;
		material.Submit(NULL);

		// 畫出模型
		g_Model_OpenGL.Render(0);
	}

	// 把影子柔化
	{
		blurred_texture = BlurTexture(g_texture);	
	}

	{
		// 使用主framebuffer object, 也就是視窗.
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		int w, h;
		GutGetWindowSize(w, h);
		glViewport(0, 0, w, h);

		// 清除畫面
		glClearColor(0.0f, 0.0f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);
		// 設定光源
		SetupLightingOpenGL();

		// 設定轉換矩陣
		Matrix4x4 world_view_matrix = world_matrix * view_matrix;

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);

		// 畫出茶壼
		g_Model_OpenGL.Render();

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);

		sModelMaterial_OpenGL material;
		material.m_Textures[0] = blurred_texture;
		material.Submit(NULL);

		// 計算貼圖矩陣
		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Identity();
		uv_offset_matrix.Scale(0.5f, 0.5f, 0.5f);
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.5f, 1.0f);

		Matrix4x4 texture_matrix = light_view_matrix * light_projection_matrix * uv_offset_matrix;

		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf( (float *)&texture_matrix);

		// 開啟自動產生貼圖座標功能
		// S/T/R分別代表貼圖座標的X/Y/Z軸
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		// 以光線的反射向量來做為貼圖座標
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

		Matrix4x4 identMatrix = Matrix4x4::IdentityMatrix();

		glTexGenfv(GL_S, GL_OBJECT_PLANE, &identMatrix[0][0]);
		glTexGenfv(GL_T, GL_OBJECT_PLANE, &identMatrix[1][0]);
		glTexGenfv(GL_R, GL_OBJECT_PLANE, &identMatrix[2][0]);

		// 畫出地表
		g_Terrain_OpenGL.Render(0);

		glLoadIdentity();
	}

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
