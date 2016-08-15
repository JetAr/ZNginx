#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElNode.h"
#include "ElNameGenerator.h"

ElNode::ElNode()
: mParent(0)
, mName("")
{
	// Generate a name
	mName = ElNameGenerator::getSingleton().generateNodeName();
}

ElNode::ElNode(const std::string& name)
: mParent(0)
,mName(name)
{

}

ElNode::~ElNode()
{

}

const std::string& ElNode::getName() const
{
	return mName;
}

void ElNode::setName(const std::string& name)
{
	mName = name;
}

const ElNode* ElNode::getParent() const
{
	return mParent;
}

void ElNode::setParent(ElNode* parent)
{
	mParent = parent;
}

void ElNode::addChild(ElNode* child)
{
	assert(child->mParent == NULL);

	mChildren.insert(ChildNodeValue(child->getName(), child));
	child->setParent(this);
}

unsigned short ElNode::numChildren() const
{
	return mChildren.size();
}

ElNode* ElNode::getChild(unsigned short index) const
{
	if (index < mChildren.size())
	{
		ChildNodeConstIterator i = mChildren.begin();
		while (--index)
			++i;
		return i->second;
	}
	else
		return NULL;
}

ElNode* ElNode::getChild(const std::string& name) const
{
	ChildNodeConstIterator i = mChildren.find(name);
	if (i == mChildren.end())
		return NULL;
	else
		return i->second;
}

ElNode* ElNode::removeChild(unsigned short index)
{
	if (index < mChildren.size())
	{
		ChildNodeIterator i = mChildren.begin();
		while (index--) ++i;	
		ElNode* ret = i->second;

		mChildren.erase(i);
		ret->setParent(NULL);
		return ret;
	}
	else
		assert(0);

	return NULL;
}

ElNode* ElNode::removeChild(ElNode* child)
{
	if (child)
	{
		ChildNodeIterator i = mChildren.find(child->getName());
		// ensure it's our child
		if (i != mChildren.end() && i->second == child)
		{
			mChildren.erase(i);
			child->setParent(NULL);
		}
	}

	return child;
}

ElNode* ElNode::removeChild(const std::string& name)
{
	ChildNodeIterator i = mChildren.find(name);
	if (i != mChildren.end())
	{
		ElNode* ret = i->second;
		mChildren.erase(i);
		ret->setParent(NULL);

		return ret;
	}

	return NULL;
}

void ElNode::removeAllChildren()
{
	ChildNodeIterator i, iend;
	iend = mChildren.end();
	for (i = mChildren.begin(); i != iend; ++i)
	{
		i->second->setParent(NULL);
	}
	mChildren.clear();
}

void ElNode::destroy()
{
	ChildNodeIterator i, iend;
	iend = mChildren.end();
	for (i = mChildren.begin(); i != iend; ++i)
	{
		i->second->destroy();
		ElSafeDelete(i->second);
	}
	mChildren.clear();
}