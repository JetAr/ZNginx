#ifndef __ElNode_H__
#define __ElNode_H__

#include <map>
#include <string>

class ElNode
{
public:
	typedef std::map<std::string, ElNode*> ChildNodeMap;
	typedef ChildNodeMap::iterator ChildNodeIterator;
	typedef ChildNodeMap::const_iterator ChildNodeConstIterator;
	typedef ChildNodeMap::value_type ChildNodeValue;

public:
	ElNode();
	ElNode(const std::string& name);
	virtual ~ElNode();

	virtual const std::string& getName() const;
	virtual void setName(const std::string& name);
	virtual const ElNode* getParent() const;
	virtual void setParent(ElNode* parent);

	virtual void addChild(ElNode* child);
	virtual unsigned short numChildren() const;
	virtual ElNode* getChild(unsigned short index) const;
	virtual ElNode* getChild(const std::string& name) const;
	virtual ElNode* removeChild(unsigned short index);
	virtual ElNode* removeChild(ElNode* child);
	virtual ElNode* removeChild(const std::string& name);
	virtual void removeAllChildren();
	virtual void destroy();

protected:
	ElNode* mParent;
	ChildNodeMap mChildren;

	std::string mName;
};

#endif //__ElNode_H__