#ifndef _GUTHEIGHTMAP_DX9_
#define _GUTHEIGHTMAP_DX9_

#include <d3d9.h>

#include "GutHeightmap.h"

class CGutHeightmapDX9 : public CGutHeightmap
{
private:

public:
	CGutHeightmapDX9(void);
	~CGutHeightmapDX9();

	virtual void Render(void);
};

#endif // _GUTHEIGHTMAP_DX9_