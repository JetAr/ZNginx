/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2010 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/
#include "mainwindow.h"


#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QMessageBox>
#include <Qt/qevent.h>//#include "OgitorsPrerequisites.h"
#include <QtCore/QTimer>

//using namespace Qt;

#include <ogre.h>
//using namespace Ogre;


#include "ogrewidget.hxx"
//#include "entityview.hxx"
//#include "layerview.hxx"
//#include "objectsview.hxx"
//#include "templateview.hxx"
//#include "propertiesviewgeneral.hxx"
//
//#include "MultiSelEditor.h"
//#include "NodeEditor.h"
//#include "EntityEditor.h"
//
//using namespace Ogitors;

extern QString ConvertToQString(Ogre::UTFString& value);

bool ViewKeyboard[1024];

//----------------------------------------------------------------------------------------
void OverlayWidget::paintEvent(QPaintEvent* evt)
{
	QPainter painter(this);
	painter.setClipRect(0,0,width(),height());
	painter.setBrush(QBrush(QColor(0,0,0)));
	painter.fillRect(QRectF(0,0,width(),height()), QColor(0,0,0));
	painter.setPen(QColor(210,210,210));
	painter.drawText(QRectF(0,0,width(),height()),msgstr,QTextOption(Qt::AlignVCenter | Qt::AlignHCenter));
}
//----------------------------------------------------------------------------------------
OgreWidget::OgreWidget(QWidget *parent, bool doLoadFile, Qt::WindowFlags f): QWidget( parent,  f /*| Qt::MSWindowsOwnDC*/ ),
 mRenderWindow(0), mOgreInitialised(false), mLastKeyEventTime(0),
mRenderStop(false), mScreenResize(false), mCursorHidden(false), mDoLoadFile(doLoadFile)
{
	mFrameCounter = 0;
	mTotalFrameTime = 0;
	mSwitchingScene = false;

	for(int i = 0;i < 1024;i++)
		ViewKeyboard[i] = false;

	setAcceptDrops(true);
	setContextMenuPolicy( Qt::PreventContextMenu );

	setFocusPolicy(Qt::WheelFocus);
	setMouseTracking(true);
	setAttribute(Qt::WA_NoBackground);
	setAttribute(Qt::WA_PaintOnScreen);

	//mOverlayWidget = new OverlayWidget(this);
	//QVBoxLayout *layout = new QVBoxLayout();
	//layout->setMargin(0);
	//layout->addWidget(mOverlayWidget);
	//setLayout(layout);

	//OgitorsRoot::getSingletonPtr()->GetProperties()->getProperty("loadstate")->connect(OgitorsSignalFunction::from_method<OgreWidget, &OgreWidget::onSceneLoadStateChange>(this), mLoadConnection);
	mogreHandle = 0;

	mOgreRoot = Ogre::Root::getSingletonPtr();

}
//----------------------------------------------------------------------------------------
OgreWidget::~OgreWidget()
{
	if(mOgreInitialised)
	{
		Ogre::Root::getSingletonPtr()->removeFrameListener(this);
		mOgreRoot->getRenderSystem()->removeListener(this);
	}

	if(mogreHandle)
	{
		delete mogreHandle;
		mogreHandle = 0;

	}

	destroy();
}
//----------------------------------------------------------------------------------------
static Ogre::Vector3 oldCamPos = Ogre::Vector3::ZERO;
int oldTris = 0;

bool OgreWidget::frameStarted(const Ogre::FrameEvent& evt)
{
	//m_robotCurrentState->addTime(evt.timeSinceLastFrame);

	//// Handle possible end of robot state if we the return to walk 
	//// flag is set indicating we return to walk mode when the existing mode ends.
	//if(m_robotReturnToWalk)
	//{
	//	if(m_robotCurrentState->getTimePosition() >= m_robotCurrentState->getLength())
	//	{
	//		SetRobotState(ID_ROBOT_WALK);
	//	}
	//}

	//m_ninjaWalkState->addTime(evt.timeSinceLastFrame);

	return true;
}
//----------------------------------------------------------------------------------------
void OgreWidget::initializeOGRE()
{
	//== Creating and Acquiring Ogre Window ==//

	// Get the parameters of the window QT created

	mOgreRoot = Ogre::Root::getSingletonPtr();

	Ogre::NameValuePairList params;

#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
	params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t) (this->winId()));
#else
	const QX11Info info = this->x11Info();
	//QX11Info info = x11Info();
	Ogre::String winHandle;
	winHandle  = Ogre::StringConverter::toString((unsigned long)(info.display()));
	winHandle += ":";
	winHandle += Ogre::StringConverter::toString((unsigned int)(info.screen()));
	winHandle += ":";
	winHandle += Ogre::StringConverter::toString((unsigned long)(this->winId()));
	winHandle += ":";
	winHandle += Ogre::StringConverter::toString((unsigned long)(info.visual()));

	params["externalWindowHandle"] = winHandle;
#endif

	mRenderWindow = mOgreRoot->createRenderWindow( "QtOgitorRenderWindow",
		this->width(),
		this->height(),
		false,
		&params );

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	mRenderWindow->resize(width(), height());
#endif

	mOgreRoot->getRenderSystem()->addListener(this);
	//OgitorsRoot::getSingletonPtr()->SetRenderWindow(mRenderWindow);
	Ogre::Root::getSingletonPtr()->addFrameListener(this);
	Ogre::MeshManager::getSingletonPtr()->setListener(this);

	mOgreInitialised = true;
}
//------------------------------------------------------------------------------------
void OgreWidget::timerLoop()
{
	if(mOgitorMainWindow->isMinimized())
		return;

	if(mRenderStop)
	{
		if(QMessageBox::information(this,"qtOgitor", tr("Render Device is Lost! Please click ok to continue.."), QMessageBox::Ok) == QMessageBox::Ok)
			mRenderStop = false;
	}
	update();
}
//------------------------------------------------------------------------------------
void OgreWidget::setDoLoadFile(bool doLoad)
{
	mDoLoadFile = doLoad;
}
//------------------------------------------------------------------------------------
bool adjustFrameTime = false;

void OgreWidget::paintEvent(QPaintEvent* evt)
{
	//if(!isVisible())
	//    return;

	QPainter painter(this);
	////painter.setClipRect(2,2,width() - 2,height() - 2);
	//painter.setBrush(QBrush(QColor(0,0,128)));
	////painter.setPen(QColor(0,0,0));
	//painter.fillRect(QRectF(0,0,width(),height()), QColor(0,0,255) );
	//painter.drawRect(10, 10, width() - 20, height() - 20);

	//if(!mRenderWindow)
	//	initializeOGRE();

	static bool firstrun = true;



	if (firstrun)
	{
		firstrun = FALSE;
		//InitOgre();
		mogreHandle = new OgreWrap;
		mogreHandle->SetupOgre(this->winId());
	}
	else
	{
		mOgreRoot->renderOneFrame();
	}

	//mOgreRoot->renderOneFrame(0.01f);
	//    if(mOgreInitialised && OgitorsRoot::getSingletonPtr()->IsSceneLoaded() && !mRenderStop)
	//    {
	//        if(this->width() > 0 && this->height() > 0)
	//        {
	//            if(OgitorsRoot::getSingletonPtr()->IsClearScreenNeeded())
	//            {
	//                Ogre::Camera *clearcam = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createCamera("sbtClearCam");
	//                clearcam->setNearClipDistance(0.0001f);
	//                clearcam->setFarClipDistance(0.0002f);
	//                clearcam->lookAt(0,-1,0);
	//                OgitorsRoot::getSingletonPtr()->GetRenderWindow()->addViewport(clearcam,0);
	//            }
	//
	//            if(adjustFrameTime)
	//            {
	//                mOgreRoot->renderOneFrame(0.01f);
	//                adjustFrameTime = false;
	//            }
	//            else
	//                mOgreRoot->renderOneFrame();
	//
	//
	//#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	//            mRenderWindow->update();
	//#endif
	//            if(OgitorsRoot::getSingletonPtr()->IsClearScreenNeeded())
	//            {
	//                OgitorsRoot::getSingletonPtr()->GetRenderWindow()->removeViewport(0);
	//                OgitorsRoot::getSingletonPtr()->GetSceneManager()->destroyCamera("sbtClearCam");
	//                OgitorsRoot::getSingletonPtr()->ClearScreenBackground(false);
	//            }
	//        }
	//    }
	//    else
	//    {
	//        QString msgstr = tr("Initializing OGRE...");
	//
	//        if(mOgreInitialised && !OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//        {
	//            if(mDoLoadFile)
	//                msgstr = tr("Loading Scene...");
	//            else
	//                msgstr = tr("Please load a Scene File...");
	//        }
	//
	//        if(mRenderStop)
	//            msgstr = tr("Device Lost...");
	//
	//        mOverlayWidget->setMessageString(msgstr);
	//
	//        adjustFrameTime = true;
	//    }
}
//------------------------------------------------------------------------------------
void OgreWidget::resizeEvent(QResizeEvent* evt)
{
	if(!mRenderWindow)
		return;

	mScreenResize = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	mRenderWindow->resize(width(), height());
#endif

	mRenderWindow->windowMovedOrResized();

	//if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    OgitorsRoot::getSingletonPtr()->RenderWindowResized();

	mScreenResize = false;
}
//------------------------------------------------------------------------------------
void OgreWidget::focusInEvent(QFocusEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	for(int i = 0;i < 1024;i++)
		ViewKeyboard[i] = false;

	evt->setAccepted(true);
}
//------------------------------------------------------------------------------------
void OgreWidget::focusOutEvent(QFocusEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	for(int i = 0;i < 1024;i++)
		ViewKeyboard[i] = false;

	//if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
	//    OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->stopEdit();

	evt->setAccepted(true);
}
//------------------------------------------------------------------------------------
void OgreWidget::keyPressEvent(QKeyEvent *evt)
{
	if(evt->isAutoRepeat())
		return;

	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	unsigned int key = evt->key();
	if(key > 255)
		key = (key & 0xFFF) + 0xFF;

	//OgitorsRoot::getSingletonPtr()->OnKeyDown(key);
}
//------------------------------------------------------------------------------------
void OgreWidget::keyReleaseEvent(QKeyEvent *evt)
{
	if(evt->isAutoRepeat())
		return;

	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	//unsigned int key = evt->key();
	//if(key > 255)
	//    key = (key & 0xFFF) + 0xFF;

	//OgitorsRoot::getSingletonPtr()->OnKeyUp(key);
}
//------------------------------------------------------------------------------------
enum OgitorsMouseButtons
{
	OMB_LEFT = 1,       /** Left mouse button type */
	OMB_RIGHT = 2,      /** Right mouse button type */
	OMB_MIDDLE = 4      /** Middle mouse button type */
};

unsigned int OgreWidget::getMouseButtons(Qt::MouseButtons buttons, Qt::MouseButton button)
{
	unsigned int flags = 0;
	buttons |= button;

	if(buttons.testFlag(Qt::LeftButton))
		flags |= OMB_LEFT;
	if(buttons.testFlag(Qt::RightButton))
		flags |= OMB_RIGHT;
	if(buttons.testFlag(Qt::MidButton))
		flags |= OMB_MIDDLE;

	return flags;
}
//------------------------------------------------------------------------------------
bool OgreWidgetMouseMovedSincePress = false;
void OgreWidget::mouseMoveEvent(QMouseEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	if(!hasFocus())
		setFocus();

	Ogre::Vector2 pos(evt->x(), evt->y());

	//OgitorsRoot::getSingletonPtr()->OnMouseMove(pos, getMouseButtons(evt->buttons(), evt->button()));

	OgreWidgetMouseMovedSincePress = true;
}
//------------------------------------------------------------------------------------
void OgreWidget::mousePressEvent(QMouseEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	//if(!hasFocus())
	//    setFocus();

	//Ogre::Vector2 pos1(evt->x(), evt->y());

	//if(evt->button() == Qt::LeftButton)
	//    OgitorsRoot::getSingletonPtr()->OnMouseLeftDown(pos1, getMouseButtons(evt->buttons(), evt->button()));
	//else if(evt->button() == Qt::RightButton)
	//    OgitorsRoot::getSingletonPtr()->OnMouseRightDown(pos1, getMouseButtons(evt->buttons(), evt->button()));
	//else if(evt->button() == Qt::MidButton)
	//    OgitorsRoot::getSingletonPtr()->OnMouseMiddleDown(pos1, getMouseButtons(evt->buttons(), evt->button()));

	//OgreWidgetMouseMovedSincePress = false;
}
//------------------------------------------------------------------------------------
void OgreWidget::mouseReleaseEvent(QMouseEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	//if(!hasFocus())
	//    setFocus();

	//Ogre::Vector2 pos(evt->x(), evt->y());

	//if(evt->button() == Qt::LeftButton)
	//    OgitorsRoot::getSingletonPtr()->OnMouseLeftUp(pos, getMouseButtons(evt->buttons(), evt->button()));
	//else if(evt->button() == Qt::RightButton)
	//    OgitorsRoot::getSingletonPtr()->OnMouseRightUp(pos, getMouseButtons(evt->buttons(), evt->button()));
	//else if(evt->button() == Qt::MidButton)
	//    OgitorsRoot::getSingletonPtr()->OnMouseMiddleUp(pos, getMouseButtons(evt->buttons(), evt->button()));

	if(!OgreWidgetMouseMovedSincePress && evt->button() == Qt::RightButton)
	{
		setContextMenuPolicy( Qt::PreventContextMenu );
		showObjectMenu();
	}
}
//------------------------------------------------------------------------------------
void OgreWidget::leaveEvent(QEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	//Ogre::Vector2 pos(-1, -1);

	//OgitorsRoot::getSingletonPtr()->OnMouseLeave(pos, 0);
	OgreWidgetMouseMovedSincePress = true;
}
//------------------------------------------------------------------------------------
void OgreWidget::wheelEvent(QWheelEvent *evt)
{
	//if(!OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
	//    return;

	if(!hasFocus())
		setFocus();

	//Ogre::Vector2 pos(evt->x(), evt->y());

	//OgitorsRoot::getSingletonPtr()->OnMouseWheel(pos, evt->delta(), getMouseButtons(evt->buttons(), Qt::NoButton));
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
QPaintEngine* OgreWidget::paintEngine() const
{
	// We don't want another paint engine to get in the way for our Ogre based paint engine.
	// So we return nothing.
	return NULL;
}

//------------------------------------------------------------------------------------
void OgreWidget::ProcessKeyActions()
{
	QTime time = QTime::currentTime();
	unsigned int curtime = time.hour() * 60 + time.minute();
	curtime = (curtime * 60) + time.second();
	curtime = (curtime * 1000) + time.msec();

	unsigned int iTimeDiff = curtime - mLastKeyEventTime;
	mLastKeyEventTime = curtime;

	//if(OgitorsRoot::getSingletonPtr()->GetViewport())
	//    OgitorsRoot::getSingletonPtr()->GetViewport()->ProcessKeyActions(iTimeDiff);
}
//------------------------------------------------------------------------------------
void OgreWidget::displayFPS(float time)
{
	mFrameCounter++;
	mTotalFrameTime += time;
	mFrameRate = (float)mFrameCounter / (float)mTotalFrameTime ;
	if(mTotalFrameTime > 2.0f)
	{
		mTotalFrameTime = 0;
		mFrameCounter = 0;
		char temp[500];
		sprintf(temp,"Ogre FPS: %.1f  ", mFrameRate);
		//mOgitorMainWindow->mFPSLabel->setText(QString(temp));
	}
}
//----------------------------------------------------------------------------------
void OgreWidget::processMaterialName(Ogre::Mesh *mesh, Ogre::String *name)
{
	//Ogre::NameValuePairList *modelMaterialMap = OgitorsRoot::getSingletonPtr()->GetModelMaterialMap();
	//modelMaterialMap->insert(Ogre::NameValuePairList::value_type(mesh->getName(), name->c_str()));
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dragEnterEvent(QDragEnterEvent *evt)
{
	void *source = (void*)(evt->source());

	//if(OgitorsRoot::getSingletonPtr()->OnDragEnter(source))
	//{
	//    evt->setDropAction(Qt::IgnoreAction);
	//    evt->acceptProposedAction();

	//    setFocus();
	//    grabKeyboard();
	//}
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dragLeaveEvent(QDragLeaveEvent *evt)
{
	//OgitorsRoot::getSingletonPtr()->OnDragLeave();
	releaseKeyboard();
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dragMoveEvent(QDragMoveEvent *evt)
{
	void *source = (void*)(evt->source());
	unsigned int modifier = evt->keyboardModifiers();

	//evt->setAccepted(OgitorsRoot::getSingletonPtr()->OnDragMove(source, modifier, evt->pos().x(), evt->pos().y()));
}
//-------------------------------------------------------------------------------------------
void OgreWidget::dropEvent(QDropEvent *evt)
{
	void *source = (void*)(evt->source());

	evt->setDropAction(Qt::IgnoreAction);

	//OgitorsRoot::getSingletonPtr()->OnDragDropped(source, evt->pos().x(), evt->pos().y());

	releaseKeyboard();
}
//-------------------------------------------------------------------------------------------
void OgreWidget::showObjectMenu()
{

}
//-------------------------------------------------------------------------------------------
void OgreWidget::objectMenu(int id)
{

}
//----------------------------------------------------------------------------------------

QMetaObject const * OgreWidget::metaObject(void)const 
{
	return QWidget::metaObject();
}

void * OgreWidget::qt_metacast(char const *)
{
	return NULL;
}

int OgreWidget::qt_metacall(enum QMetaObject::Call,int,void * *)
{
	return 0;
}

QMetaObject const OgreWidget::staticMetaObject;

void OgreWidget::CreateScene(Ogre::SceneManager *pSceneManager)
{

	//pSceneManager->setAmbientLight( ColourValue( 1.0f, 1.0f, 1.0f ) );

	//m_robotEntity = pSceneManager->createEntity("robot", "robot.mesh");

	//// Add entity to the scene node
	//// Place and rotate to face the Z direction
	//Vector3 robotLoc(30, 0, 0);
	//Quaternion robotRot(Degree(-90), Vector3(0, 1, 0));

	//pSceneManager->getRootSceneNode()->createChildSceneNode(robotLoc, robotRot)->attachObject(m_robotEntity);

	//m_robotWalkState = m_robotEntity->getAnimationState("Walk");
	//m_robotWalkState->setEnabled(true);

	//// Set the initial state
	//m_robotState = ID_ROBOT_WALK;
	//m_robotCurrentState = m_robotWalkState;
	//m_robotReturnToWalk = false;

	////
	//// Create the ninja entity
	////

	//Entity *ent = pSceneManager->createEntity("ninja", "ninja.mesh");

	//// Add entity to the scene node
	//// Place and rotate to face the Z direction
	//Vector3 ninjaLoc(-30, 0, 0);
	//Quaternion ninjaRot(Degree(180), Vector3(0, 1, 0));

	//SceneNode *ninjaNode = pSceneManager->getRootSceneNode()->createChildSceneNode(ninjaLoc, ninjaRot);
	//ninjaNode->scale(0.5, 0.5, 0.5);        // He's twice as big as our robot...
	//ninjaNode->attachObject(ent);

	//m_ninjaWalkState = ent->getAnimationState("Walk");
	//m_ninjaWalkState->setEnabled(true);
}


//
//void OgreWidget::SetRobotState(int p_state)
//{   
//	// 
//	// Determine what we are doing from and what we are going to
//	//
//
//	Ogre::AnimationState *oldState = m_robotCurrentState;
//	Ogre::AnimationState *newState;
//
//	m_robotState = p_state;
//	m_robotReturnToWalk = false;      // Default is not to return to walk
//
//	switch(m_robotState)
//	{
//	case ID_ROBOT_WALK:
//		newState = m_robotWalkState;
//		break;
//
//	case ID_ROBOT_SHOOT:
//		newState = m_robotEntity->getAnimationState("Shoot");
//		newState->setLoop(false);              // We'll not loop this state
//		m_robotReturnToWalk = true;         // Return to walking when done
//		break;
//
//	case ID_ROBOT_IDLE:
//		newState = m_robotEntity->getAnimationState("Idle");
//		break;
//
//	case ID_ROBOT_SLUMP:
//		newState = m_robotEntity->getAnimationState("Slump");
//		newState->setLoop(false);              // We'll not loop this state
//		break;
//
//	case ID_ROBOT_DIE:
//		newState = m_robotEntity->getAnimationState("Die");
//		newState->setLoop(false);              // We'll not loop this state
//		break;
//	}
//
//	m_robotCurrentState = newState;
//
//	//
//	// Disable the old state
//	//
//
//	oldState->setEnabled(false);
//
//	// 
//	// Enable the new state
//	//
//
//	newState->setEnabled(true);
//	newState->setTimePosition(0);
//}
