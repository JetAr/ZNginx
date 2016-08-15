#ifndef _GUTHEIGHTMAP_OPENGL_
#define _GUTHEIGHTMAP_OPENGL_

#include "GutHeightmap.h"

class CGutHeightmapOpenGL : public CGutHeightmap
{
private:

public:
	CGutHeightmapOpenGL(void);
	~CGutHeightmapOpenGL();

	virtual void Render(void);
};

#endif // _GUTHEIGHTMAP_OPENGL_