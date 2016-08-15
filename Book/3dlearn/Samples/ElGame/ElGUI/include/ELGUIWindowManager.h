#ifndef __ELGUIWindowManager_H__
#define __ELGUIWindowManager_H__

#include "ELGUISingleton.h"
#include "ELGUIWindow.h"
#include <map>
#include <string>

namespace ELGUI
{
	class WindowManager : public Singleton<WindowManager>
	{
	public:
		WindowManager();
		virtual ~WindowManager();

		static WindowManager& getSingleton();
		static WindowManager* getSingletonPtr();

		Window* createWindow(const std::string type, const std::string& name = "");
		
		void destroyWindow(Window* window);
		void destroyWindow(const std::string& window);
		void destroyAllWindows();
		
		Window*	getWindow(const std::string& name) const;
		bool isWindowPresent(const std::string& name) const;

		std::string WindowManager::generateUniqueWindowName();

	protected:
		typedef std::map<std::string, Window*> WindowRegistry;
		WindowRegistry			d_windowRegistry;

		unsigned long			d_uid_counter;
	};
}

#endif //__ELGUIWindowManager_H__