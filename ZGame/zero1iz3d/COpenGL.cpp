// COpenGL.cpp -


#include "main.h"



COpenGL::COpenGL()
{
    memset( states, 0, sizeof(states)*FREQUENT_STATES );
    memset( state_was, 0, sizeof(states)*FREQUENT_STATES );
    mode2d = false;
}


COpenGL::~COpenGL()
{

}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Initialize()
//// Desc: inicializira opengl i view-funkciite
/////////////////////////////////////////////////////////////////////////////////////////////////
int COpenGL::Initialize( int width, int height, HWND hwnd )
{

    GLuint   PixelFormat;
    static   PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
        1,								// version Number
        PFD_DRAW_TO_WINDOW |			// Format Must Support Window
        PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,				// Must Support Double Buffering
        PFD_TYPE_RGBA,					// Request An RGBA Format
        16,								// Select Our Color Depth
        0, 0, 0, 0, 0, 0,				// Color Bits Ignored
        0,								// No Alpha Buffer
        0,								// Shift Bit Ignored
        0,								// No Accumulation Buffer
        0, 0, 0, 0,						// Accumulation Bits Ignored
        16,								// 16Bit Z-Buffer (Depth Buffer)
        0,								// No Stencil Buffer
        0,								// No Auxiliary Buffer
        PFD_MAIN_PLANE,					// Main Drawing Layer
        0,								// Reserved
        0, 0, 0
    };


    // take window and dc handles
    m_hWindow = hwnd;
    m_hDeviceContext = GetDC( m_hWindow );
    if ( !m_hDeviceContext )
    {
        // appendtolog
        return FALSE;
    }

// opitai da namerish podhodqsht pixel-format
    if ( !(PixelFormat = ChoosePixelFormat( m_hDeviceContext, &pfd )) )
    {
        // append to log
        Destroy();
        return FALSE;
    }

    SetPixelFormat( m_hDeviceContext, PixelFormat, &pfd );

// wzemi contex za rendirane
    m_hRenderContext = wglCreateContext( m_hDeviceContext );
// activirai contexta
    wglMakeCurrent( m_hDeviceContext, m_hRenderContext );


    setState( gl_TEXTURE_2D, true );
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );

    glClearDepth( 1.0f );
    glDepthFunc(GL_LEQUAL);
    setState( gl_DEPTH_TEST, true );

    glFrontFace( GL_CCW );	 // izkarva polygonite obratno na 4asovnikovata strelka
    glCullFace( GL_BACK );  // enable culling of back-facing polys
    glEnable(GL_CULL_FACE); // ne izvyrshva izchisleniq za light za FRONT i BACK polygoni
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
//glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );	// anti-alising
//glEnable( GL_POINT_SMOOTH );

//glScissor( 10, 10, 530, 400 );			// ogranichawane risuvaneto po ekrana
//glEnable( GL_SCISSOR_TEST );

// setupni perspectivata i ekrana
    ResizeScene( width, height );


//************************************ // {!}
    /*
     GLfloat    light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };  // ambient e svetlina koqto ogrqwa wsi4ki predmeti
    GLfloat    light_diffuse[] = { 1.0f, 1.0f, 1.0f };  // diffuse ogrqwa samo tezi kym koito e nasochena
    GLfloat    light_pos[] = { 4.0f, 4.0f, 1.0f, 1.0f }; // poziciq na svetlinata (x,y,z-positivno chislo=po-napred kym teb ;)
    GLfloat    light_spotpos[] = { 128.0f, 128.0f, 0.0f }; // poziciq na svetlinata (x,y,z,w-positivno chislo=po-napred kym teb ;)
    														// w = 0 directional, w <> 0 positional
    /*
      glLightfv( GL_LIGHT1, GL_POSITION, light_pos );
      glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
      glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
      //glLightfv( GL_LIGHT0, GL_SPECULAR, light_ambient );
      glEnable( GL_LIGHT0 );

    */

// LATEST

    /* BOOK */
    /*

     GLfloat position[] = {0, 10, 0, 1};
     glLightfv( GL_LIGHT0, GL_POSITION, position);

     GLfloat ambient[]= {0.2, 0.2, 0.3};
     glLightfv( GL_LIGHT0, GL_AMBIENT, ambient);

     GLfloat diffuse[]= {1, 1, 1};
     glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse);

     GLfloat specular[]= {1,1,1};
     glLightfv( GL_LIGHT0, GL_SPECULAR, specular);

     GLfloat spotdir[]= {0,-1,0};
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotdir);

     glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 65.0f);

     // material
     GLfloat aambient[]= {0.1, 0.1, 0.1, 1};
     glMaterialfv( GL_FRONT, GL_AMBIENT, aambient);

     GLfloat adiffuse[]= {1, 1, 1, 1};
     glMaterialfv( GL_FRONT, GL_DIFFUSE, adiffuse);

     GLfloat aspecular[]= {1, 1, 1, 1};
     glMaterialfv( GL_FRONT, GL_SPECULAR, aspecular);

     glMaterialf( GL_FRONT, GL_SHININESS, 25.0f);

    */

    glEnable( GL_LIGHT0 );
    //glEnable( GL_LIGHTING );

    float light_ambient[]  = { 0.1f, 0.1f, 0.1f, 0.1f };
    float light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 0.0f };
    float light_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float mat_shininess[]  = { 25.0 };
    float light_dir[] = {0.0f, -1.0f, 0.0f };
    float light_pos[] = { 0.0f, 10.0f, 0.0f, 1.0f};

    glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
    glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, light_dir );
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 75.0f );

    // zarade tova izcezvat textovete
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );    // sets lighting to one-sided

    glEnable(GL_COLOR_MATERIAL);

    GLfloat mat_ambient[]= { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[]= { 1.0f, 1.0f, 1.0f, 1.0f };

    glColorMaterial( GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );


    //glLightModeli(GL_LIGHT_MODEL_AMBIENT, GL_TRUE);
    //	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    /*glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
    glLightModelfv(GL_DIFFUSE, light_diffuse);
    glLightModelfv(GL_SPECULAR, light_specular);
    glLightModelf (GL_SHININESS, light_shininess);*/

    //glEnable(GL_NORMALIZE);

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: ResizeScene()
//// Desc:
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::ResizeScene( GLsizei width, GLsizei height )
{

    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );				 // selectni Projection matricata (otgovarq za perspectivata)
    glLoadIdentity();

//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 4 , 4000.0f);
    gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 200.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    m_screen_width = width;
    m_screen_height = height;

}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Destroy()
//// Desc: release-va promenlivite i zatvarq GL
/////////////////////////////////////////////////////////////////////////////////////////////////
int COpenGL::Destroy()
{

// detachni rendering contexta ot device contexta
    if ( m_hRenderContext )
    {

        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( m_hRenderContext );
        m_hRenderContext = NULL;
    }

// proveri device contexta
    if ( m_hDeviceContext )
    {
        ReleaseDC( m_hWindow, m_hDeviceContext );
        m_hDeviceContext = NULL;
    }

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: BeginScene()
//// Desc: Izchistva buffera
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::BeginScene()
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: EndScene()
//// Desc: swap-va buferite
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::EndScene()
{

    glFlush(); // uverqva che komadnite shte se izpalnet wmesto da se syhranqt w buffer

    SwapBuffers( m_hDeviceContext );
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Set2D()
//// Desc: set 2D ortho mode
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::set2D()
{

    // get projection matrix
    glMatrixMode( GL_PROJECTION );
    // push it
    glPushMatrix();
    // reset projection
    glLoadIdentity();
    // set ortho mode
    glOrtho( 0, m_screen_width, m_screen_height, 0, -1.0, 1.0 );
    // reset model-view matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    setState( gl_DEPTH_TEST, false );

    mode2d = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Set3D()
//// Desc:
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::set3D()
{
    if ( mode2d )
    {
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );

        setState( gl_DEPTH_TEST, true );
        mode2d = false;
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: setState()
//// Desc: set various GL states
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::setState( GL_STATES state, bool enable )
{
    bool en;

    // remember old value
    state_was[state] = states[state];

    if ( enable )
    {
        if ( !states[state] )
            states[state] = en = true;
        else
            return;
    }
    else
    {
        if ( states[state] )
            states[state] = en = false;
        else
            return;
    }

    // enable/disable state
    switch( state )
    {
    case gl_TEXTURE_2D:
        if ( en ) glEnable( GL_TEXTURE_2D );
        else glDisable( GL_TEXTURE_2D );
        break;
    case gl_DEPTH_TEST:
        if ( en ) glEnable( GL_DEPTH_TEST );
        else glDisable( GL_DEPTH_TEST );
        break;
    case gl_LIGHTING:
        if ( en ) glEnable( GL_LIGHTING );
        else glDisable( GL_LIGHTING );
        break;
    case gl_LIGHT0:
        if ( en ) glEnable( GL_LIGHT0 );
        else glDisable( GL_LIGHT0 );
        break;
    case gl_LIGHT1:
        if ( en ) glEnable( GL_LIGHT1 );
        else glDisable( GL_LIGHT1 );
        break;
    case gl_LIGHT2:
        if ( en ) glEnable( GL_LIGHT2 );
        else glDisable( GL_LIGHT2 );
        break;
    case gl_LIGHT3:
        if ( en ) glEnable( GL_LIGHT3 );
        else glDisable( GL_LIGHT3 );
        break;
    case gl_LIGHT4:
        if ( en ) glEnable( GL_LIGHT4 );
        else glDisable( GL_LIGHT4 );
        break;
    case gl_LIGHT5:
        if ( en ) glEnable( GL_LIGHT5 );
        else glDisable( GL_LIGHT5 );
        break;
    case gl_LIGHT6:
        if ( en ) glEnable( GL_LIGHT6 );
        else glDisable( GL_LIGHT6 );
        break;
    case gl_LIGHT7:
        if ( en ) glEnable( GL_LIGHT7 );
        else glDisable( GL_LIGHT7 );
        break;
    case gl_BLEND:
        if ( en ) glEnable( GL_BLEND );
        else glDisable( GL_BLEND );
        break;
    case gl_ALPHA_TEST:
        if ( en ) glEnable( GL_ALPHA_TEST );
        else glDisable( GL_ALPHA_TEST );
        break;
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: restoreState()
//// Desc: restore old state var
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::restoreState( GL_STATES state )
{
    if ( !state_was[state] ) setState( state, false );
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: getViewMat()
//// Desc: get view matrix
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::getViewMat( CMatrix &m )
{
    float mv[16];

    glGetFloatv( GL_MODELVIEW_MATRIX, mv );
    m = CMatrix( mv );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: getProjectionMat()
//// Desc: get projection matrix
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::getProjectionMat( CMatrix &m )
{
    float mp[16];

    glGetFloatv( GL_PROJECTION_MATRIX, mp );
    m = CMatrix( mp );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: setViewMat()
//// Desc: set new view matrix
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::setViewMat( CMatrix m )
{
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixf( m.mat );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: setProjectionMat()
//// Desc: set projection matrix
/////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGL::setProjectionMat( CMatrix m )
{
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixf( m.mat );
}



/******************* CLASS CDISPLAYLIST ********************/


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Begin()
//// Desc: start building display list
/////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int CDisplayList::Begin()
{

    unsigned int list_id;

    list_id = glGenLists( 1 );

    // on success, start a new list and add it to local array
    if ( list_id != 0U )
    {
        glNewList( list_id, GL_COMPILE );
        dl.push_back( list_id );

        return list_id;
    }

    return 0U;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: End()
//// Desc: finish building Dlist
/////////////////////////////////////////////////////////////////////////////////////////////////
void CDisplayList::End()
{
    glEndList();
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Render()
//// Desc: render a list id
/////////////////////////////////////////////////////////////////////////////////////////////////
void CDisplayList::Render( unsigned int list_id )
{
    glCallList( list_id );
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Destroy()
//// Desc: Destroy all lists
/////////////////////////////////////////////////////////////////////////////////////////////////
void CDisplayList::Destroy()
{
    if ( dl.empty() )
        return;

    int sz = (int)dl.size();

    // delelte lists from GL
    for( int i = 0; i < sz; i++ )
        glDeleteLists( dl[i], 1 );

    // clear vector
    dl.clear();
}
