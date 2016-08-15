#ifndef _GUTIL_
#define _GUTIL_

#include <windows.h>
#include <GL/gl.h>
#include <d3d9.h>
#include <string>

#include "GutDefs.h"
#include "GutMem.h"
#include "GutDX9.h"
#include "GutDX10.h"
#include "GutOpenGL.h"

#include "Vector4.h"
#include "Matrix4x4.h"

extern GutCallBack g_GutCallBack;

void GutGetMouseState(int &x, int &y, int button[3]);
bool GutCreateWindow(int x, int y, int width, int height, const char *title = "Gut Application");
bool GutFullScreen(int width, int height);
bool GutCreateWindow(GutDeviceSpec &spec);
void GutEnterMessageLoop(void);
bool GutProcessMessage(void);
void GutResizeFunc(  void (*resize)(int x, int y) );
void GutDisplayFunc( void (*onpaint)(void) );
void GutIdleFunc( void (*onidle)(void) );
void GutOnCloseFunc( void (*onclose)(void) );
bool GutInitGraphicsDevice(const char *device);
bool GutInitGraphicsDevice(GutDeviceSpec &spec);
bool GutReleaseGraphicsDevice(void);

void GutSetShaderPath(const char *path);
const char *GutGetShaderPath(void);

GutEnum GutGetGraphicsDeviceType(const char *device = NULL);

Matrix4x4 GutMatrixLookAtRH(Vector4 &eye, Vector4 &lookat, Vector4 &up);
Matrix4x4 GutMatrixLookAtLH(Vector4 &eye, Vector4 &lookat, Vector4 &up);

Matrix4x4 GutMatrixOrthoLH(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixOrthoRH(float w, float h, float z_near, float z_far);

Matrix4x4 GutMatrixPerspectiveRH(float fovy, float aspect, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveLH(float fovy, float aspect, float z_near, float z_far);

Matrix4x4 GutMatrixOrtho_DirectX(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixOrthoRH_DirectX(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixOrthoLH_DirectX(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveRH_DirectX(float fovy, float aspect, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveRH_DirectX(float fovy, float aspect, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveLH_DirectX(float fovy, float aspect, float z_near, float z_far);

Matrix4x4 GutMatrixOrtho_OpenGL(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixOrthoRH_OpenGL(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixOrthoLH_OpenGL(float w, float h, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveRH_OpenGL(float fovy, float aspect, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveRH_OpenGL(float fovy, float aspect, float z_near, float z_far);
Matrix4x4 GutMatrixPerspectiveLH_OpenGL(float fovy, float aspect, float z_near, float z_far);

// 產生格子的函式
bool GutCreateGrids(int x_grids, int y_grids, 
					Vertex_V3N3 **ppVertices, int &num_vertices, 
					unsigned short **ppIndices, int &num_indices,
					int &num_triangles);

// 釋放格子模型資源
bool GutReleaseGrids(Vertex_V3N3 **ppVertices, unsigned short **ppIndices);

void GutConvertColor(Vector4 &vColor, unsigned char rgba[4]);

void GutPushMessage(const char *msg);
std::string GutPopMessage(void);

#endif // _GUTIL_