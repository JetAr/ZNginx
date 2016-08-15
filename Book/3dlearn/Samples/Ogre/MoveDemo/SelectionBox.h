#ifndef __SelectionBox_h_
#define __SelectionBox_h_

#include "OgreManualObject.h"

class SelectionBox : public Ogre::ManualObject
{
public :
	SelectionBox(const Ogre::String& name);
	~SelectionBox(void);

	void setCorners(float left, float top, float right, float bottom);
	void setCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& topRight);
};

#endif