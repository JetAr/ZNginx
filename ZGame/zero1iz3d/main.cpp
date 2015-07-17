// OPEN_GL programa N:3 - Zarejdane i 'lepene' na Texturi



#include "main.h"
#include "3ds.h"


#define APP_NAME    "Zero1_IZ3D"
#define APP_ID      "Zero1_IZ3D CLASS"
#define FULLSCREEN  1


//--- vars------------------------------------------------
HGLRC      hRC = NULL;        // opengl kontekst za rendirane (swyrzwa se s Device contextan na Windows)
HDC		   hDC = NULL;        // windowski Device context
HWND	   hWnd = NULL;       // globalna promenliva za prozorteza
HINSTANCE  hInst;		      // instance
bool	   bActive = true;	  // dali programata e activna (minimizrane ili ne)
bool	   bRunning = false;
bool       key[256];		  // masiv ot flagova za natisnati kopcheta
bool	   bRender_Terrain = true;
bool	   bRender_Grid = true;

GLfloat    xrot,yrot,zrot;	  // ygyl na vyrtene za predmeta
GLuint     texture[6];	      // masiv texturi


COpenGL   Cgl;
CCamera   camera;
CLoad3DS  g_loadmodel;		  // 3D Modela
CLoad3DS  g_loadmodel2;		  // 3D Modela
CModel    g_3Dmodel;
CModel    g_terrain;
CText2D   text;

CSprite3D sprite3;
CSprite3D sprite33;
CBillBoard cbb;

CDisplayList cdl;
unsigned int uintSky;
CTimer	  tmr;
CGameFrame  frm;
//CDebug	  debugger;

CParticleRandom pc;
CTexture tgrass;

CBoundBox bb_cube;			// bound box na kuba

CFog fog;					// magla

float wx=0, wy=-1.0, wz=0;

//--- function prototypes --------------------------------
LONG WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
int DrawGLScene(void);
int InitGLWindow(int width, int height);
GLvoid ResizeGLScene( GLsizei width, GLsizei height );
GLvoid KillGLWindow(void);
AUX_RGBImageRec *LoadBMP( char *filename );
int LoadGLTextures(void);
void RenderModel();
bool ChangeToFullScreen();
//--------------------------------------------------------




// wsichko koeto shte se pokazwa na ekrana e *tuk*
int DrawGLScene(void)
{

    CVector v;

    frm.Update();  // update timeframe

    Cgl.BeginScene();


// write text
//glPushMatrix();
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -1.0 );
    glColor3f( 0.1f, 0.1f, 0.1f );
    camera.setPosition2D( 0.0f, 0.2f );
    text << "p_x:" << camera.mv_pos.x;
    text << " p_y:" << camera.mv_pos.y;
    text << " p_z:" << camera.mv_pos.z;
    camera.setPosition2D( 0.0f, 0.22f );
    text << "v_x:" << camera.mv_view.x;
    text << " v_y:" << camera.mv_view.y;
    camera.setPosition2D( 0.0f, 0.24f );
    text << "FPS:" << frm.getFps();
    camera.setPosition2D( 0.0f, 0.26f );
    text << "MPF:" << frm.getMpf();
// glPopMatrix();



//gluPerspective( 100.0, 1.0, 0.0, 10.0 );
    gluLookAt( camera.mv_pos.x, camera.mv_pos.y, camera.mv_pos.z,
               camera.mv_view.x, camera.mv_view.y, camera.mv_view.z,
               camera.mv_up.x, camera.mv_up.y, camera.mv_up.z );


// light keys
    if ( key['U'] ) Cgl.setState( gl_LIGHTING, true );
    if ( key['I'] ) Cgl.setState( gl_LIGHTING, false );

// fog keys
    if ( key['N'] ) fog.setEnable();
    if ( key['M'] ) fog.setDisable();
    if ( key['B'] ) fog.Initialize( FOG_EXP2, rgb( 0.5, 0.5, 1.0f ), 2.0f, 7.0f, 0.05f );
    if ( key['V'] ) fog.setLOD( false );

    if ( key['G'] ) camera.Rotate( 0.005f, CVector( 1.0f, 0.0f, 0.0f ) );  // wy += 0.5f;
    if ( key['H'] ) camera.Rotate( -0.005f, CVector( 1.0f, 0.0f, 0.0f ) ); // wy -= 0.5f;
    if ( key['Z'] ) wy += 10.0f * frm.getMpf();
    if ( key['C'] ) wy -= 10.0f * frm.getMpf();
// ( key['W'] ) wz += 0.1f;
// ( key['S'] ) wz -= 0.1f;

    if ( key['Q'] ) camera.Rotate( 2.5f * frm.getMpf(), CVector( 0.0f, 1.0f, 0.0f ) );
    if ( key['E'] ) camera.Rotate( -2.5f * frm.getMpf(), CVector( 0.0f, 1.0f, 0.0f ) );

    if ( key['W'] ) camera.Move( 0.5f * frm.getMpf());
    if ( key['S'] ) camera.Move( -0.5 * frm.getMpf() );

//if ( key['W'] ) if ( !bb_cube.Collide( camera.mv_pos ) ) camera.Move( 0.005f );
//if ( key['S'] ) if ( !bb_cube.Collide( camera.mv_pos ) ) camera.Move( -0.005f );
    if ( key['A'] ) camera.Strafe( -2.5f * frm.getMpf() );
    if ( key['D'] ) camera.Strafe( 2.5f * frm.getMpf() );

// render-dbg
    if ( key['O'] ) bRender_Terrain = false;
    if ( key['P'] ) bRender_Terrain = true;
    if ( key['K'] ) bRender_Grid = false;
    if ( key['L'] ) bRender_Grid = true;

// vyrtene na perspectivata
    if ( key['R'] ) xrot += 0.2f;
    if ( key['T'] ) xrot -= 0.2f;
    glRotatef( xrot, 0.0f, 1.0f, 0.0f );

    /*if ( bb_cube.Collide( camera.mv_pos ) )
    {
     camera.setPosition2D( 0.0f, 0.28f );
     text << "COLLIDE";
    }*/

    glTranslatef( wx, wy, wz );



    glPushMatrix();

    float w = 2.7f, z = 0.0f;
    int i,j;

    glTranslatef( 0.0, 0.0, 0.0 );
    if ( bRender_Terrain )
        g_terrain.Render();

    glTranslatef( -13.0, 0.0, -16.0 );

    Cgl.setState( gl_TEXTURE_2D, true );
    tgrass.Activate();

    /*glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -lwidth, -lheight );  // lower left
    glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -lwidth, lheight );   // lower right
    glTexCoord2f( 1.0f, 0.0f ); glVertex2f( lwidth, lheight );    // upper right
    glTexCoord2f( 1.0f, 1.0f ); glVertex2f( lwidth, -lheight );   // upper left*/
// draw tile terrain

    glBegin( GL_QUADS );

    for (  j = 0; j < 10; j++ )
        for (  i = 0; i < 10; i++ )
        {
            glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

            v = CVector( (float)(i*w), z, (float)(j*w) );
            v.glNormal();
            glTexCoord2f(0.0f, 1.0f);
            v.glVector();


            v = CVector( (float)(i*w), z, (float)(j*w+w) );
            v.glNormal();
            glTexCoord2f(0.0f, 0.0f);
            v.glVector();

            v = CVector( (float)(i*w+w), z, (float)(j*w+w) );
            v.glNormal();
            glTexCoord2f(1.0f, 0.0f);
            v.glVector();

            v = CVector( (float)(i*w+w), z, (float)(j*w) );
            v.glNormal();
            glTexCoord2f(1.0f, 1.0f);
            v.glVector();

        }

    glEnd();
    glPopMatrix();


// render/update particles
    glPushMatrix();
    pc.Update( Cgl, frm.getMpf() );
    glPopMatrix();

// render sky
    /*glPushMatrix();
    glTranslatef( camera.mv_pos.x, camera.mv_pos.y, camera.mv_pos.z  );
    glColor3f( 1.0f, 1.0f, 1.0f );
    cdl.Render( uintSky );
    glPopMatrix();
     */

// render models
    glPushMatrix();
    /*glBindTexture( GL_TEXTURE_2D, texture[0] );
    glTranslatef( camera.mv_view.x , -1.0, camera.mv_view.z );*/

    glTranslatef( -7, 0.0f, 0 );
    g_3Dmodel.Render();

    glTranslatef( 7, 1.0f, 5.0 );
    cbb.Reneder( Cgl );

// render IBR tree
    sprite3.setPosition( CVector( -3.0, 0.0, -9.0 ) );
    sprite3.Render( Cgl );
    sprite33.setPosition( CVector( -3.0, 0.0, -9.0 ) );
    sprite33.Rotate( CVector( 0.0, 90.0f, 0.0f ) );
    sprite33.Render( Cgl );



    /*for( float g = 0.0, int k = 0; k < 36; k++, g += 0.01f )
    {
    glTranslatef( -7, 0.0f, 7 + g);
    g_3Dmodel.Render();
    }*/


    glPopMatrix();


// Draw Grid
    if ( bRender_Grid )
    {
        for(float k = -150; k <= 150; k += 5)
        {
            glBegin(GL_LINES);
            glColor3ub(15, 15, 15);
            glVertex3f(-150, 0, k);
            glVertex3f(150, 0, k);
            glVertex3f(k, 0,-150);
            glVertex3f(k, 0, 150);
            glEnd();
        }
    }

    /*


      glPushMatrix();

        glScalef(0.5f, 0.5f, 0.5f);
    	glTranslatef(0,1.0f,0);
    	glBegin(GL_QUADS);
    		glColor3f(0.0f,1.0f,0.0f);
    		glVertex3f( 1.0f, 1.0f,-1.0f);
    		glVertex3f(-1.0f, 1.0f,-1.0f);
    		glVertex3f(-1.0f, 1.0f, 1.0f);
    		glVertex3f( 1.0f, 1.0f, 1.0f);
    		glColor3f(1.0f,0.5f,0.0f);
    		glVertex3f( 1.0f,-1.0f, 1.0f);
    		glVertex3f(-1.0f,-1.0f, 1.0f);
    		glVertex3f(-1.0f,-1.0f,-1.0f);
    		glVertex3f( 1.0f,-1.0f,-1.0f);
    		glColor3f(1.0f,0.0f,0.0f);
    		glVertex3f( 1.0f, 1.0f, 1.0f);
    		glVertex3f(-1.0f, 1.0f, 1.0f);
    		glVertex3f(-1.0f,-1.0f, 1.0f);
    		glVertex3f( 1.0f,-1.0f, 1.0f);
    		glColor3f(1.0f,1.0f,0.0f);
    		glVertex3f( 1.0f,-1.0f,-1.0f);
    		glVertex3f(-1.0f,-1.0f,-1.0f);
    		glVertex3f(-1.0f, 1.0f,-1.0f);
    		glVertex3f( 1.0f, 1.0f,-1.0f);
    		glColor3f(0.0f,0.0f,1.0f);
    		glVertex3f(-1.0f, 1.0f, 1.0f);
    		glVertex3f(-1.0f, 1.0f,-1.0f);
    		glVertex3f(-1.0f,-1.0f,-1.0f);
    		glVertex3f(-1.0f,-1.0f, 1.0f);
    		glColor3f(1.0f,0.0f,1.0f);
    		glVertex3f( 1.0f, 1.0f,-1.0f);
    		glVertex3f( 1.0f, 1.0f, 1.0f);
    		glVertex3f( 1.0f,-1.0f, 1.0f);
    		glVertex3f( 1.0f,-1.0f,-1.0f);
    	glEnd();



      glPopMatrix();

      sprite3.setPosition( CVector( 0.0, 0.5, 0.0 ) );
      //sprite3.setRotate( true );
     // sprite3.setRotationSpeed( CVector( .5f, 1.0f, .2f ) );
      sprite3.Render();

      // draw 2d
    /*  Cgl.set2D();

      sprite.Rotate( -45.0f );

      //sprite.SetAlpha( true );
      sprite.setPosition( CVector2D( 50, 100 ) );

      //sprite.SetRotationSpeed( 1.0f );
      //sprite.SetRotate( true );

        sprite.Render();

    	glBegin(GL_QUADS);
    		glColor3f(1.0f,1.0f,1.0f);
    		glVertex2f( 50.0f, 50.0f );
    		glVertex2f( 100.0f, 50.0f );
    		glVertex2f( 100.0f, 100.0f );
    		glVertex2f( 50.0f, 100.0f );
    	glEnd();

     Cgl.set3D();
     */

    Cgl.EndScene();


    return true;
}


// zarejda texturite
int LoadGLTextures(void)
{

    int status = false;
    AUX_RGBImageRec *image[6];
    char *texfiles[] = { "data/cliffs.bmp", "data/body.bmp"};

    memset( image, NULL, sizeof(image[0])*3 );
//memset( image[0], 0, sizeof(void *)*1 );

    int i = 0;
    for (i = 0; i < 2; i++ )
    {

        // zaredi bitmap
        if (image[i] = LoadBMP( texfiles[i] ))
        {
            status = true;
            // param1 - kolko texturi da se generirat ( v sluchaq 1 )
            // param2 - masiva za syhranenieto im
            glGenTextures( 1, &texture[i] );
            // opredelq imeto na texturata s syotvetnata informaciq (bitmap inforamciqta)
            glBindTexture( GL_TEXTURE_2D, texture[i] );
            // sega zaredi infoto ot bitmap-a v texturata
            /*glTexImage2D( GL_TEXTURE_2D, 0, 3, image[i]->sizeX, image[i]->sizeY, 0, GL_RGB,
            		   GL_UNSIGNED_BYTE, image[i]->data );*/
            // filtrirane na texturata - kak da se mapva kogato obekta e blizo/dalech ot ekrana
            // GL_LINEAR - bavno no po-dobro, GL_NEAREST - po-byrzo
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST  );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            gluBuild2DMipmaps( GL_TEXTURE_2D, 3, image[i]->sizeX, image[i]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image[i]->data );

        }
    }


    for ( i = 0; i < 2; i++ )
    {
        // osvobodi zaetata pamet
        if ( image[i] )
        {
            if ( image[i]->data ) free(image[i]->data);
            free(image[i]);
        }
    }

    return status;
}



// zaredi bitmap
AUX_RGBImageRec *LoadBMP( char *filename )
{

    FILE *fp = NULL;

    // proveri parametyra (hah!)
    if ( !filename ) return NULL;

    // proveri dali file-a syshtestvuva i go zaredi ako e taka!
    if ( (fp = fopen( filename, "r" ) ))
    {
        fclose(fp);
        return auxDIBImageLoad(filename);
    }

    return NULL;
}


// Fulscreen mode
bool ChangeToFullScreen( int width, int height, int bits )
{

    DEVMODE dmScreenSettings;                                 // Device Mode
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));   // Makes Sure Memory's Cleared
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings); // Size Of The Devmode Structure
    dmScreenSettings.dmPelsWidth  = width;                    // Selected Screen Width
    dmScreenSettings.dmPelsHeight = height;                   // Selected Screen Height
    dmScreenSettings.dmBitsPerPel = bits;                     // Selected Bits Per Pixel
    dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

    // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
        // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
        if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",
                       "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
        {
            return false;
            //FullScreen = false; // Windowed Mode Selected.  Fullscreen = false
        }
        else
        {
            // Pop Up A Message Box Letting User Know The Program Is Closing.
            MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);

            return false; // Return false
        }
    }

    return true;

}


LONG WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

    switch( msg )
    {
    case WM_ACTIVATE:
    {
        if ( !HIWORD(wParam) )    // proverka za minimizrana
        {
            bActive = true;
        }
        else
        {
            bActive = false;
        }
        return 0L;
    }

    case WM_KEYUP:
        key[wParam] = false;
        break;


    case WM_KEYDOWN:

        key[wParam] = true;

        if ( wParam == VK_F12 )
        {
            PostMessage ( hWnd, WM_CLOSE, NULL, NULL );
            return 0L;
        }
        break;

    case WM_SIZE:
        Cgl.ResizeScene( LOWORD(lParam), HIWORD(lParam) );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    return 0L;

}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    WNDCLASSEX  wcex;
    MSG		  msg;
    BOOL		  bMsgCame;


    // create class
    wcex.cbSize	     = sizeof(wcex);
    wcex.style	     = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
    wcex.lpszClassName = APP_ID;
    wcex.lpfnWndProc   = (WNDPROC)WndProc;
    wcex.lpszMenuName  = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); //GetStockObject(BLACK_BRUSH);
    wcex.hCursor       = (HCURSOR)LoadCursor( hInst, IDC_ARROW );
    wcex.hIcon		 = LoadIcon( NULL, IDI_WINLOGO );
    wcex.hIconSm		 = LoadIcon( NULL, IDI_WINLOGO );
    wcex.hInstance     = hInstance;
    wcex.cbClsExtra	 = 0;
    wcex.cbWndExtra	 = 0;

    if ( !RegisterClassEx( &wcex ) ) return E_FAIL;

    if ( FULLSCREEN )
    {
        ChangeToFullScreen( 800, 600, 16 );

        // create app-window --- FULSCREEN
        hWnd = CreateWindowEx( WS_EX_LEFT, APP_ID, APP_NAME,
                               WS_POPUP,
                               0, 0, 800, 600,
                               NULL, NULL, hInst, NULL );
    }
    else
    {
        // create app-window --- WINDOW MODE
        hWnd = CreateWindowEx( WS_EX_LEFT | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, APP_ID, APP_NAME,
                               WS_OVERLAPPEDWINDOW,
                               0, 0, 800, 600,
                               NULL, NULL, hInst, NULL );
    }

    if ( hWnd == NULL ) return E_FAIL;

    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    hInst = hInstance;


    /*************/
    if ( !Cgl.Initialize( 800, 600, hWnd ) )
    {
        MessageBox( NULL, "Greshka v incializirane na OpenGL", NULL, MB_OK | MB_ICONINFORMATION );
        return -1;
    }

    __texttype tth;
    tth.fontname = "Arial";
    tth.bold = false;
    tth.underlined = false;
    tth.italic = false;
    tth.size = 12;
    tth.zv = 0;

    text.Setup( Cgl, tth );

    // load sprites3d
    sprite3.Load( "data/tree2.tga" );
    sprite33.Load( "data/tree2.tga" );
    sprite3.setAlpha( ALPHABLEND_NORMAL );
    sprite33.setAlpha( ALPHABLEND_NORMAL );
    tgrass.LoadTextureFromTGA( "data/grass.tga" );

    // setup billboards
    CBillBoardFragment bbf;
    bbf.setSize( 2.0, 2.0 );
    bbf.setPosition( CVector( 0.0, 0.0, 0.0 ) );
    cbb.setTexture( "data/tree2.tga" );
    cbb.Add( bbf );
    // billboard manager
    bbf.setPosition( CVector( 5.0, 0.0, 3.0 ) );
    cbb.Add( bbf );

    LoadGLTextures();

    // Load modelite
    g_loadmodel.Import3DS( &g_3Dmodel, "data/warrior.3ds" );
    g_loadmodel2.Import3DS( &g_terrain, "data/terren3z.3ds" );


    //camera.setPosition( 25, 10.0, 30, 0, 0, 0, 0, 1, 0 ); // -> perspective
    camera.setPosition( 0, -0.5, 10, 0, -1, 0, 0, 1, 0 );

    /*
      if ( !(uintSky = cdl.Begin()) ) retusrn -11;
      // draw sky -> store as a DISPLAY-LIST
      glPushMatrix();
      glTranslatef( camera.mv_pos.x, camera.mv_pos.y, camera.mv_pos.z  );
      glBindTexture( GL_TEXTURE_2D, texture[2] );
      GLUquadric *nq = gluNewQuadric();
      gluQuadricTexture( nq, true );
      gluQuadricDrawStyle( nq, GLU_FILL );
      glRotatef( 90.0, 1.0f, 0.0f, 0.0f );
      gluSphere( nq, 90.0f, 10, 10 );	// radius = FOV+x !!! za da ne se skrivat obektite
      glPopMatrix();
      cdl.End();
    */


    // setup timer and frame counter
    tmr.Start();
    frm.setTimer( &tmr );

    // setup particles
    pc.Initialize( CVector( 0.0f, 0.0f, 0.0f ) );

    // setup bound_box
    bb_cube.setDimensions( CVector( 2.0f, 2.0f, 2.0f ) );
    bb_cube.setPosition( CVector( -1.0f, -1.0f, -1.0f ) );

    // setup fog
    //fog.Initialize( FOG_LINEAR, rgb( 0.5, 0.5, 0.5 ), 2.0, 6.0 );
    fog.Initialize( FOG_EXP2, rgb( 0.5, 0.5, 1.0f ), 2.0f, 7.0f, 0.05f );
    fog.setEnable();
    bRender_Grid = false;
    Cgl.setState( gl_LIGHTING, true );


    //debugger.Throw( __FILE__, __LINE__, "Oh, god" );

    /****************/

// wzemi 1woto syobshtenie
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
    bRunning = true;

    while(  msg.message != WM_QUIT   )
    {


        bMsgCame = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );


        // process syobshteniqta
        if ( bMsgCame )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            DrawGLScene();
        }


    } // krai na cikyla

    g_3Dmodel.Destroy();
    pc.Destroy();
    cbb.Destroy();

    Cgl.Destroy();
    return (int)msg.wParam;
    //return 0;

}
