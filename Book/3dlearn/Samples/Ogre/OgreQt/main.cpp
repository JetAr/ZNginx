#include <QtGui/QApplication>

#include "MainWindow.h"

#include <Ogre.h>
Ogre::Root           *mOgreRoot;
BOOL InitOgre();

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//OgreQt w;
	//w.show();

	InitOgre();


	mOgitorMainWindow = new MainWindow();
	mOgitorMainWindow->show();
	mOgitorMainWindow->setApplicationObject(&a);


	mOgitorMainWindow->repaint();


	int retval = a.exec();


	delete mOgitorMainWindow;


	return retval;
}


BOOL InitOgre()
{
	using namespace Ogre;

#ifdef _DEBUG
	mOgreRoot = new Ogre::Root("plugins_d.cfg", "OgreMFC.cfg", "OgreMFC.log"); 
#else
	mOgreRoot = new Ogre::Root("plugins.cfg", "OgreMFC.cfg", "OgreMFC.log"); 
#endif

	//
	// Setup paths to all resources
	//

	Ogre::ConfigFile cf;
	cf.load("resources_d.cfg");

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, secName);
		}
	}

	const RenderSystemList& render =  mOgreRoot->getAvailableRenderers();

	RenderSystemList::const_iterator pRend = render.begin();

	while (pRend != render.end())
	{
		Ogre::String rName = (*pRend)->getName();
		//if (rName == "OpenGL Rendering Subsystem") //
		if (rName == "Direct3D9 Rendering Subsystem") //Direct3D9 Rendering Subsystem
			break;
		pRend++;
	}

	if (pRend == render.end())
	{
		// Unrecognised render system
		//MessageBox("Unable to locate OpenGL rendering system.  Application is terminating");
		return FALSE;
	}

	Ogre::RenderSystem *rsys = *pRend;
	rsys->setConfigOption("Full Screen", "No");
	rsys->setConfigOption("VSync", "Yes");

	// Set the rendering system.
	mOgreRoot->setRenderSystem(rsys);

	//
	// Initialize the system, but don't create a render window.
	//
	mOgreRoot->initialise(false);

	return TRUE;
}