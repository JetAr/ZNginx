#ifndef __ElEntity_H__
#define __ElEntity_H__

#include "ElDefines.h"
#include "ElSceneNode.h"
#include "ElMovableObject.h"
#include "ElMesh.h"
#include "ElSubEntity.h"
#include "ElSkeleton.h"
#include "ElAnimation.h"
#include "ElAxisAlignedBox.h"
#include "ElDummyObject.h"
#include "ElRenderable.h"
#include <string>
#include <map>

class ElEntity : public ElMovableObject, public ElRenderable
{
public:
	union FrameList
	{
	   ElSkeletalAnimation*				skeletonFrames;
	   ElMeshAnimation*					meshFrames;

	   FrameList()	: skeletonFrames(NULL), meshFrames(NULL) {}
	};
	struct AnimationSet
	{
		ElAnimation*	animation;
		FrameList		frames;

		AnimationSet() { animation = ElNew ElAnimation; }
		~AnimationSet() { ElSafeDelete(animation); }
	};

	typedef std::map<std::string, ElDummyObject*> ChildObjectMap;
	typedef ChildObjectMap::iterator ChildObjectIterator;
	typedef ChildObjectMap::const_iterator ConstChildObjectIterator;
	typedef ChildObjectMap::value_type ChildObjectValue;

public:
	ElEntity(ElMesh* mesh);
	virtual ~ElEntity();

	static ElEntity*	createEntity(LPCSTR filename);

	virtual void		setAttached(ElSceneNode* parent);

	virtual void		attachSkeleton(ElSkeleton* skeleton);
	virtual bool		hasAnimation();

	virtual float		getAlpha() const;
	virtual void		setAlpha(float alpha);

	virtual void		render(ElCamera* camera, TimeValue t);

	virtual void		setAnimation(const std::string& name);
	virtual void		playAnimation(TimeValue t, bool loop = true);
	virtual void		setAnimationSpeed(float speed);
	virtual void		stopAnimation();

	virtual const		ElAxisAlignedBox& getWorldBoundingBox(bool derive = false);
	virtual const		ElAxisAlignedBox& getBoundingBox();

	virtual ElSceneNode* getChildObject(const std::string& name);

	ElMesh*				getMesh();

	virtual void		_updateRenderQueue(ElRenderQueue* queue, ElCamera* cam);
	virtual float		getSquaredViewDepth(ElCamera* cam) const;

protected:
	virtual void		buildSubEntityList();
	virtual void		buildChildObjects();
	virtual void		updateAnimation(TimeValue t);
	virtual void		_setRenderStates();
	virtual void		_resetRenderStates();

	virtual void		_notifyAttached(ElSceneNode* parent);
	virtual void		_notifySkeletonAttached(ElSkeleton* skeleton);
	virtual ElAxisAlignedBox getChildWorldBoundingBox();

protected:
	ElMesh*				mMesh;
	AnimationSet*		mAnimation;
	
	ElSubEntityList		mSubEntityList;
	ChildObjectMap		mChildObjectList;

	ElAxisAlignedBox	mFullBoundingBox;

	float				mAlpha;

	// vertex blend mode(whether to use software processing), only used when the entity
	// has a skeleton attached
	bool				mUseSoftwareVP;
};

#endif //__ElEntity_H__