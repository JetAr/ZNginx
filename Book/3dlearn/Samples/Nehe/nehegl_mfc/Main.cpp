/****************************************
*   Nehe MFC by Yvo van Dillen based on *
*   Jeff Molofee's Basecode Example     *
*          nehe.gamedev.net             *
*             2001/2004                 *
*                                       *
*****************************************/

#include "stdafx.h"
#include "NeheMFC.h"
#include "NeheWindow.h"
#include "Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMain::CMain()
{
	// Start Of User Initialization	
	angle= rot1 = rot2 = 0 ;
}

CMain::~CMain()
{

}

// Function name	: CMain::KeyPressed
// Description	    : Checks for a key press
//                    if( KeyPressed(VK_ESCAPE) ) { ... Escape was pressed ... }
// Return type		: BOOL 
// Argument         : int nCode
BOOL CMain::KeyPressed(int nCode)
{
	if( nCode >= 0 && nCode <= 255 )
	{
		return theApp.keyDown[ nCode ];
	}
	return FALSE;
}


// Function name	: CMain::Initialize
// Description	    : This function will initialize your opengl application
//					  Put in what you need
// Return type		: BOOL 
//					  return TRUE on success or FALSE on error (example :texture/model not found = FALSE)
BOOL CMain::Initialize()
{

	if (!LoadGLTextures())								// Jump To Texture Loading Routine ( NEW )
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	return TRUE;
}


// Function name	: CMain::Deinitialize
// Description	    : This function will Deinitialize your opengl application
//					  destroy all objects here
// Return type		: void 
void CMain::Deinitialize()
{

}


// Function name	: CMain::Update
// Description	    : this function will be called every xx milliseconds
//					  update your variables through milliseconds
// Return type		: void 
// Argument         : DWORD milliseconds
void CMain::Update(DWORD milliseconds)
{	
	if (KeyPressed(VK_ESCAPE) == TRUE)						// Is ESC Being Pressed?
	{
		theApp.TerminateApplication ();						// Terminate The Program
	}

	if (KeyPressed(VK_F1) == TRUE)							// Is F1 Being Pressed?
	{		
		theApp.ToggleFullScreen ();							// Toggle Fullscreen Mode
	}

	if (KeyPressed(VK_SPACE ) == TRUE)							// Is F1 Being Pressed?
	{		
		theApp.ToggleFullScreen ();							// Toggle Fullscreen Mode
	}


	angle += (float)(milliseconds) / 5.0f;					// Update angle Based On The Clock
}


// Function name	: CMain::Draw
// Description	    : this function will draw (blt) your opengl scene to the window
// Return type		: void 
	//glRotatef (angle, 0.0f, 1.0f, 0.0f);						// Rotate On The Y-Axis By angle
	//glTranslatef (0.0f, 0.0f, -6.0f);

void CMain::Draw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The View
	glTranslatef(0.0f,0.0f,-5.0f);

	glRotatef(angle,1.0f,0.0f,0.0f);
	glRotatef(angle,0.0f,1.0f,0.0f);
	glRotatef(angle,0.0f,0.0f,1.0f);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
	// Front Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	// Back Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	// Top Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	// Bottom Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	// Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	// Left Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();


	glFlush ();													// Flush The GL Rendering Pipeline
}


void CMain::DrawAxis()
{

	glBegin(GL_LINES);	
	glColor3f(1.0f , 0.0f,0.0f);
	glVertex3f(-0, 0.0f, 0.0f);	
	glColor3f(0.0f , 0.0f,0.0f);
	glVertex3f( 4.0f, 0.0f, 0.0f);	

	glColor3f(0.0f , 1.0f,0.0f);
	glVertex3f( 0.0f,0.0, 0.0f);	
	glColor3f(0.0f , 0.0f,0.0f);
	glVertex3f(0.0f,2, 0.0f);		

	glColor3f(0.0f , 0.0f,1.0f);
	glVertex3f( 0.0f,0.0f, 0 );	
	glColor3f(0.0f , 0.0f,0.0f);
	glVertex3f(0.0f,0.0f, 4.0f );		

	glEnd();		


}

int CMain::LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	int Status=FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP("Data/cube.bmp"))
	{
		Status=TRUE;									// Set The Status To TRUE

		glGenTextures(1, &texture[0]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, \
			GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}

	if (TextureImage[0])									// If Texture Exists
	{
		if (TextureImage[0]->data)							// If Texture Image Exists
		{
			free(TextureImage[0]->data);					// Free The Texture Image Memory
		}

		free(TextureImage[0]);								// Free The Image Structure
	}

	return Status;										// Return The Status
}

AUX_RGBImageRec *CMain::LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}