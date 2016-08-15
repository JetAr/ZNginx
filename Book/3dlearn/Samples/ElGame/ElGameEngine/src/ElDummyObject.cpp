#include "ElApplicationPCH.h"
#include "ElDummyObject.h"

ElDummyObject::ElDummyObject(const std::string& name)
: mParentName("")
, ElSceneNode(name)
{

}

ElDummyObject::~ElDummyObject()
{

}

void ElDummyObject::setParentName(const std::string& name)
{
	mParentName = name;
}

const std::string& ElDummyObject::getParentName()
{
	return mParentName;
}

void ElDummyObject::setInitScale(const D3DXVECTOR3& scale)
{
	mInitScale = scale;
}

void ElDummyObject::setInitQrientation(const D3DXQUATERNION& q)
{
	mInitOrientation = q;
}

void ElDummyObject::setInitPosition(const D3DXVECTOR3& pos)
{
	mInitPosition = pos;
}

void ElDummyObject::setAttachedBone(ElBone* bone)
{
	mBone = bone;
}

ElBone* ElDummyObject::getAttachedBone()
{
	return mBone;
}

void ElDummyObject::updateFromParentImpl()
{
	// do update in bone coordination first
	if (mBone)
	{
		// calculate the transform according to the bone, then
		// decompose it into rotation, scale and position
		D3DXQUATERNION q;
		D3DXVECTOR3 s, p;
		D3DXMatrixDecompose(&s, &q, &p, &mBone->mTransform);

		// update orientation from bone to node coordination
		mDerivedOrientation = mInitOrientation * q * mOrientation;

		// update scale from bone to node coordination
		mDerivedScale.x = mInitScale.x * s.x * mScale.x;
		mDerivedScale.y = mInitScale.y * s.x * mScale.y;
		mDerivedScale.z = mInitScale.z * s.x * mScale.z;

		// update position from bone to node coordination
		D3DXVECTOR3 pos = mInitPosition + p;
		pos.x = mScale.x * pos.x;
		pos.y = mScale.y * pos.y;
		pos.z = mScale.z * pos.z;
		D3DXMATRIX mat;
		D3DXMatrixRotationQuaternion(&mat, &mDerivedOrientation);
		D3DXVec3TransformCoord(&pos, &pos, &mat);
		
		mDerivedPosition = pos + mPosition;
	}
	else
	{
		mDerivedOrientation = mOrientation;
		mDerivedPosition = mPosition;
		mDerivedScale = mScale;
	}

	// then do update in node coordition
	if (mParent)
	{
		ElSceneNode* parent = static_cast<ElSceneNode*>(mParent);

		// update orientation
		const D3DXQUATERNION& parentOrientation = parent->getDerivedOrientation();
		if (mInheritOrientation)
		{
			// Combine orientation with that of parent
			mDerivedOrientation = parentOrientation * mDerivedOrientation;
		}

		// update scale
		const D3DXVECTOR3& parentScale = parent->getDerivedScale();
		if (mInheritScale)
		{
			// Scale own position by parent scale, NB just combine
			// as equivalent axes, no shearing
			mDerivedScale.x = parentScale.x * mDerivedScale.x;
			mDerivedScale.y = parentScale.x * mDerivedScale.y;
			mDerivedScale.z = parentScale.x * mDerivedScale.z;
		}

		// Change position vector based on parent's orientation & scale
		D3DXVECTOR3 pos;
		pos.x = parentScale.x * mDerivedPosition.x;
		pos.y = parentScale.y * mDerivedPosition.y;
		pos.z = parentScale.z * mDerivedPosition.z;
		D3DXMATRIX m;
		D3DXMatrixRotationQuaternion(&m, &parentOrientation);
		D3DXVec3TransformCoord(&mDerivedPosition, &pos, &m);

		// Add altered position vector to parents
		mDerivedPosition += parent->getDerivedPosition();
	}
}
