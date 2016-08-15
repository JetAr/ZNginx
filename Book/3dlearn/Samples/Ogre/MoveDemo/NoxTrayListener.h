#pragma once
//using namespace OgreBites;

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>

class NoxTrayListener :
	public OgreBites::SdkTrayListener
{
public:
	NoxTrayListener(void);
	virtual ~NoxTrayListener(void);
};
