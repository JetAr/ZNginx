// main.h -


/*

  KenamicK Entertainment 3D Engine 
  
  Engine Title: 'Zero1_IZ3D'
  
  © 2004 Mar, April, May
  
  http://www.kenamick.com/

  -> v proces na razrabotka LoL: 
 ---------------------- points to follow
  * setting GL STATES e time-consuming	       -> opraveno s frequent lista v COpenGL.cpp
  * Polzvai Display Lists za static models     -> class completed
  * VAs are best for no more than 1000 triangles
  * glColor3ub() faster than glColor4f() !?!?!
  * {!} da vnesa pracalculiranite sin i cos v CCamera.cpp clasa
*/

#ifndef  __MAIN_H__
#define  __MAIN_H__

#ifndef STRICT
#define STRICT
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>


using namespace std;

#define rndf(x)   (((float)rand()/(float)RAND_MAX) * x)
//#define RENDERER    COpenGL&

extern float g_cos[];
extern float g_sin[];


// engine
#include "Geometry.h"
#include "COpenGL.h"
#include "CTexture.h"
#include "CCamera.h"
#include "3ds.h"
#include "CModel.h"
#include "CText.h"
#include "CSprite.h"
#include "CParticle.h"
#include "CBillBoard.h"
#include "CFog.h"
// system 
#include "CTimer.h"
#include "CDebug.h"

#endif // __MAIN_H__
