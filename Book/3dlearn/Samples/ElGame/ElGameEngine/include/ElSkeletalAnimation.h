#ifndef __ElSkeletalAnimation_H__
#define __ElSkeletalAnimation_H__

#include "ElD3DHeaders.h"
#include "ElDefines.h"
#include "ElArray.h"
#include <vector>

struct ElDecomposedTM
{
	D3DXVECTOR3 scale;
	D3DXQUATERNION rotation;
	D3DXVECTOR3 translation;

	ElDecomposedTM() : scale(1.0f, 1.0f, 1.0f), translation(0.0f, 0.0f, 0.0f)
	{
		D3DXQuaternionIdentity(&rotation);
	}
};

class ElSkeletalAnimation
{
public:
	struct Frame
	{
		TimeValue				t;
		ElArray<ElDecomposedTM> dm;
		ElArray<D3DXMATRIX>		tm;
		ElArray<int>			boneId;
	};
	typedef std::vector<Frame*> SkinFrameList;

	ElSkeletalAnimation();
	virtual ~ElSkeletalAnimation();

	bool			load(LPCTSTR filename);

	void			addFrame(Frame* frame);
	Frame*			getFrame(int idx);
	int				getNumFrames();

protected:
	SkinFrameList	mFrameList;
};

#endif //__ElSkeletalAnimation_H__