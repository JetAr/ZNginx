﻿#ifndef _CSHADER_H
#define _CSHADER_H


// This is a define that we use for our function pointers
#define APIENTRYP APIENTRY *

// Here we include the vertex and fragment shader defines
#define GL_VERTEX_SHADER_ARB              0x8B31
#define GL_FRAGMENT_SHADER_ARB            0x8B30

#define GL_VERTEX_PROGRAM_ARB             0x8620
#define GL_FRAGMENT_PROGRAM_ARB           0x8804

#define GL_OBJECT_COMPILE_STATUS_ARB	  0x8B81
#define GL_OBJECT_LINK_STATUS_ARB		  0x8B82
#define GL_OBJECT_INFO_LOG_LENGTH_ARB	  0x8B84
#define GL_OBJECT_DELETE_STATUS_ARB		  0x8B80

// These are for our multi-texture defines
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4

#define GL_DEPTH_TEXTURE_MODE_ARB         0x884B
#define GL_TEXTURE_COMPARE_MODE_ARB       0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB       0x884D
#define GL_COMPARE_R_TO_TEXTURE_ARB       0x884E

// This is what GL uses for handles when using extensions
typedef unsigned int GLhandleARB;
typedef char GLcharARB;

// Below are all of our function pointer typedefs for all the extensions we need
typedef GLhandleARB (APIENTRYP PFNGLCREATESHADEROBJECTARBPROC) (GLenum shaderType);
typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC) (GLhandleARB shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC) (GLhandleARB shaderObj);
typedef GLhandleARB (APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC) (void);
typedef void (APIENTRYP PFNGLATTACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB obj);
typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
typedef void (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC) (GLhandleARB programObj);
typedef void (APIENTRYP PFNGLUNIFORM1IARBPROC) (GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1FARBPROC) (GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FARBPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);
typedef void (APIENTRYP PFNGLDETACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB attachedObj);
typedef void (APIENTRYP PFNGLDELETEOBJECTARBPROC) (GLhandleARB obj);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (APIENTRYP PFNGLGETOBJECTPARAMETERIV) (GLhandleARB obj, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETINFOLOGARBPROC) (GLhandleARB obj, GLsizei maxLength, GLsizei* length, GLcharARB *infoLog);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRYP PFNGLACTIVETEXTUREARBPROC) (GLenum target);


// Here we extern our functions to be used elsewhere
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
extern PFNGLUNIFORM1FARBPROC glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC glUniform4fARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
extern PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
extern PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLGETOBJECTPARAMETERIV glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC glGetInfoLogARB;


// This is our very basic shader class that we will use
class CShader
{
public:

    // Create an empty constructor and have the deconstructor release our memory.
    CShader()	{				}
    ~CShader()
    {
        Release();
    }

    // This loads our text file for each shader and returns it in a string
    string LoadTextFile(string strFile);

    // This loads a vertex and fragment shader
    void InitShaders(string strVertex, string strFragment);

    // This returns an ID for a variable in our shader
    GLint GetVariable(string strVariable);

    // These are our basic get functions for our private data
    GLhandleARB GetProgram()
    {
        return m_hProgramObject;
    }
    GLhandleARB GetVertexS()
    {
        return m_hVertexShader;
    }
    GLhandleARB GetFragmentS()
    {
        return m_hFragmentShader;
    }

    // Below are functions to set an integer or a set of floats
    void SetInt(GLint variable, int newValue)
    {
        glUniform1iARB(variable, newValue);
    }
    void SetFloat(GLint variable, float newValue)
    {
        glUniform1fARB(variable, newValue);
    }
    void SetFloat2(GLint variable, float v0, float v1)
    {
        glUniform2fARB(variable, v0, v1);
    }
    void SetFloat3(GLint variable, float v0, float v1, float v2)
    {
        glUniform3fARB(variable, v0, v1, v2);
    }
    void SetFloat4(GLint variable, float v0, float v1, float v2, float v3)
    {
        glUniform4fARB(variable, v0, v1, v2, v3);
    }

    // These 2 functions turn on and off our shader
    void TurnOn()
    {
        glUseProgramObjectARB(m_hProgramObject);
    }
    void TurnOff()
    {
        glUseProgramObjectARB(0);
    }

    // This releases our memory for our shader
    void Release();

private:

    // This handle stores our vertex shader information
    GLhandleARB m_hVertexShader;

    // This handle stores our fragment shader information
    GLhandleARB m_hFragmentShader;

    // This handle stores our program information which encompasses our shader
    GLhandleARB m_hProgramObject;
};


// This is used to load all of the extensions and checks compatibility.
bool InitGLSL();


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// Nothing new was added to this file for this tutorial.
//
//
// © 2000-2005 GameTutorials
