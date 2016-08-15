#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElBone.h"

ElBone::ElBone()
: mParentName("")
{
	D3DXMatrixIdentity(&mInitTM);
	D3DXMatrixIdentity(&mInverseInitTM);
	D3DXMatrixIdentity(&mTransform);
}

ElBone::~ElBone()
{

}

void ElBone::update()
{
	if (mParent)
		mTransform *= static_cast<ElBone*>(mParent)->mTransform;
	else
	{
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		mTransform *= mat;
	}

	ElBone* child = 0;
	for (ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i)
		static_cast<ElBone*>(i->second)->update();

	// set mTransform as offset transform to its original TM, avoid that the
	// application has to calculate it every time the bone is used.
	mTransform = mInverseInitTM * mTransform;
}
