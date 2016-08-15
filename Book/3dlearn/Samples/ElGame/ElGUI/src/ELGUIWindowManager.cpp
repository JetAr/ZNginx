#include "ELGUIPCH.h"
#include "ELGUIWindowManager.h"
#include "ELGUIWindowFactoryManager.h"
#include "ELGUISystem.h"
#include <sstream>

namespace ELGUI
{
	template<> WindowManager* Singleton<WindowManager>::ms_Singleton = 0;

	WindowManager::WindowManager()
	: d_uid_counter(0)
	{

	}

	WindowManager::~WindowManager()
	{
		destroyAllWindows();
	}

	WindowManager& WindowManager::getSingleton()
	{
		return Singleton<WindowManager>::getSingleton();
	}

	WindowManager* WindowManager::getSingletonPtr()
	{
		return Singleton<WindowManager>::getSingletonPtr();
	}

	Window* WindowManager::createWindow(const std::string type, const std::string& name /* = "" */ )
	{
		std::string finalName(name);
		if (finalName.empty())
		{
			finalName = generateUniqueWindowName();
		}

		if (isWindowPresent(finalName))
		{
			assert(0);
			return getWindow(finalName);
		}

		WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
		WindowFactory* factory = wfMgr.getFactory(type);
		assert(factory);
		
		Window* newWindow = factory->createWindow(finalName);
		d_windowRegistry[finalName.c_str()] = newWindow;

		return newWindow;
	}

	void WindowManager::destroyWindow(Window* window)
	{
		if (window)
		{
			std::string name = window->getName();
			destroyWindow(name);
		}
	}

	void WindowManager::destroyWindow(const std::string& window)
	{
		WindowRegistry::iterator wndpos = d_windowRegistry.find(window.c_str());

		if (wndpos != d_windowRegistry.end())
		{
			Window* wnd = wndpos->second;

			// remove entry from the WindowRegistry.
			d_windowRegistry.erase(wndpos);

			// do 'safe' part of cleanup
			wnd->destroy();

			// delete wnd instance
			WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
			WindowFactory* factory = wfMgr.getFactory(wnd->getType());
			assert(factory);
			factory->destroyWindow(wnd);

			// notify system object of the window destruction
			System::getSingleton().notifyWindowDestroyed(wnd);
		}
	}

	void WindowManager::destroyAllWindows()
	{
		std::string window_name;
		while (!d_windowRegistry.empty())
		{
			window_name = d_windowRegistry.begin()->first;
			destroyWindow(window_name);
		}
	}
	
	Window* WindowManager::getWindow(const std::string& name) const
	{
		WindowRegistry::const_iterator pos = d_windowRegistry.find(name.c_str());

		if (pos == d_windowRegistry.end())
		{
			assert(0);
			return 0;
		}

		return pos->second;
	}

	bool WindowManager::isWindowPresent(const std::string& name) const
	{
		return (d_windowRegistry.find(name.c_str()) != d_windowRegistry.end());
	}

	std::string WindowManager::generateUniqueWindowName()
	{
		// build name
		std::ostringstream uidname;
		uidname << "__elgui_uid_" << d_uid_counter;

		// update counter for next time
		unsigned long old_uid = d_uid_counter;
		++d_uid_counter;

		// log if we ever wrap-around (which should be pretty unlikely)
		if (d_uid_counter < old_uid)
			assert(0 && "UID counter for generated window names has wrapped around - the fun shall now commence!");

		// return generated name as a CEGUI::String.
		return uidname.str();
	}
}