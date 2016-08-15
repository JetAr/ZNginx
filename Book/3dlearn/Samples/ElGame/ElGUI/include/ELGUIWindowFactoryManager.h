#ifndef __ELGUIWindowFactoryManager_H__
#define __ELGUIWindowFactoryManager_H__

#include "ELGUISingleton.h"
#include "ELGUIWindowFactory.h"
#include <map>
#include <string>

#define ELGUI_CREATE_WINDOW_FACTORY(T) new T ## Factory(#T)

namespace ELGUI
{
	class WindowFactoryManager : public Singleton<WindowFactoryManager>
	{
	public:
		WindowFactoryManager();
		virtual ~WindowFactoryManager();

		static WindowFactoryManager& getSingleton();
		static WindowFactoryManager* getSingletonPtr();

		void addFactory(WindowFactory* factory);
		WindowFactory* getFactory(const std::string& type) const;
		void destroyAllFactory();

	protected:
		typedef	std::map<std::string, WindowFactory*> WindowFactoryRegistry;
		WindowFactoryRegistry	d_factoryRegistry;
	};
}

#endif //__ELGUIWindowFactoryManager_H__