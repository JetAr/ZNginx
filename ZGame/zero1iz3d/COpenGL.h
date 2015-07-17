// COpenGl.h -


#ifndef __COPENGL_INCLUDED
#define __COPENGL_INCLUDED



/////////////////////
class COpenGL;
class CDisplayList;
/////////////////////


typedef COpenGL& RENDERER;


// alphblending states
typedef enum
{
    ALPHABLEND_NONE = 0,
    ALPHABLEND_NORMAL,
    ALPHABLEND_ADDITIVE
} ALPHABLENDING;


#define FREQUENT_STATES 13

// frequently called states
typedef enum
{
    gl_TEXTURE_2D = 0,
    gl_DEPTH_TEST,
    gl_LIGHTING,
    gl_LIGHT0,
    gl_LIGHT1,
    gl_LIGHT2,
    gl_LIGHT3,
    gl_LIGHT4,
    gl_LIGHT5,
    gl_LIGHT6,
    gl_LIGHT7,
    gl_BLEND,
    gl_ALPHA_TEST
} GL_STATES;


// OpenGL Class
class COpenGL
{

private:
    HWND       m_hWindow;
    HDC		   m_hDeviceContext;
    HGLRC	   m_hRenderContext;								// opengl kontekst za rendirane
    int		   m_screen_width, m_screen_height;
    bool	   mode2d;											// are we in 2D viewport mode

    // cache frequently called states
    bool	   states[FREQUENT_STATES];							// current state
    bool	   state_was[FREQUENT_STATES];						// the state before the current one

public:

    COpenGL();
    ~COpenGL();

    // init/release
    int Initialize( int , int , HWND );
    void ResizeScene(  GLsizei , GLsizei );
    int Destroy();

    void BeginScene();
    void EndScene();

    // states
    void set2D();
    void set3D();
    void setState( GL_STATES state, bool enable );
    bool getState( GL_STATES state )
    {
        return states[state];
    };
    bool getStateWas( GL_STATES state )
    {
        return state_was[state];
    };
    void restoreState( GL_STATES state );

    // math
    void getViewMat( CMatrix &m );
    void getProjectionMat( CMatrix &m );
    void setViewMat( CMatrix m );
    void setProjectionMat( CMatrix m );

    // retrieve
    HDC GetHDC() const
    {
        return m_hDeviceContext;
    };


};


// Display Lists Class
class CDisplayList
{

private:
    vector<unsigned int> dl;

public:

    CDisplayList()
    {
        dl.clear();
    };
    ~CDisplayList()
    {
        Destroy();
    };

    unsigned int Begin();
    void End();

    void Render( unsigned int );
    void Destroy();

};


// Color class
typedef struct rgb
{
    float r, g, b;
    rgb()
    {
        r = g = b = 1.0f;
    };
    rgb( float red, float green, float blue )
    {
        r = red;
        g = green;
        b = blue;
    };
    rgb( const rgb &c )
    {
        r = c.r;
        g = c.g;
        b = c.b;
    };
    rgb& operator=( const rgb &c )
    {
        r = c.r;
        g = c.g;
        b = c.b;
        return *this;
    };

} rgb;

// Alpha color class
typedef struct rgba
{
    float r, g, b, a;
    rgba()
    {
        r = g = b = 1.0f;
        a = 1.0f;
    };
    rgba( float red, float green, float blue )
    {
        r = red;
        g = green;
        b = blue;
        a = 1.0f;
    };
    rgba( float red, float green, float blue, float alpha )
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    };
    rgba( const rgba &c )
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    };
    rgba( const rgb &c )
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = 1.0f;
    };
    rgba& operator=( const rgba &c )
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    };

} rgba;

#define BLACK  rgb( 0.0f, 0.0f, 0.0f )
#define WHITE  rgb( 1.0f, 1.0f, 1.0f )


#endif
