#pragma once

//
// This is a simple class for rendering robots in Ogre
//
#include <Ogre.h>



class CScene : public Ogre::FrameListener
{
public:
	CScene(void);
	~CScene(void);

	void CreateScene(Ogre::SceneManager *pSceneManager);

	void SetRobotState(int p_state);
	int  GetRobotState() const {return m_robotState;}

	virtual bool frameStarted(const Ogre::FrameEvent& evt);

private:    
	int     m_robotState;
	bool    m_robotReturnToWalk;

	Ogre::Entity         *m_robotEntity;
	Ogre::AnimationState *m_robotWalkState;
	Ogre::AnimationState *m_robotCurrentState;

	Ogre::AnimationState *m_ninjaWalkState;
};
