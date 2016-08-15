#ifndef __ElShader_H__
#define __ElShader_H__

#include "ElD3DHeaders.h"
#include "ElCamera.h"
#include "ElLight.h"

class ElShader
{
public:
	ElShader();
	virtual ~ElShader();

	virtual bool create(LPCSTR shaderFileName);
	virtual void render(ElCamera* camera);
	
protected:
	virtual bool initialize();
	virtual void renderImp(ElCamera* camera);
	virtual void setShaderClipPlane(ElCamera* camera, bool shaderMode);
	
protected:
	ID3DXEffect*			mEffect;
};

#endif __ElShader_H__