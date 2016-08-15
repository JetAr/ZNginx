#include "ELGUIPCH.h"
#include "ELGUIWindow.h"
#include "ELGUIWindowFactoryManager.h"

namespace ELGUI
{
	template<> WindowFactoryManager* Singleton<WindowFactoryManager>::ms_Singleton = 0;

	WindowFactoryManager::WindowFactoryManager()
	{

	}

	WindowFactoryManager::~WindowFactoryManager()
	{
		destroyAllFactory();
	}

	WindowFactoryManager& WindowFactoryManager::getSingleton()
	{
		return Singleton<WindowFactoryManager>::getSingleton();
	}

	WindowFactoryManager* WindowFactoryManager::getSingletonPtr()
	{
		return Singleton<WindowFactoryManager>::getSingletonPtr();
	}

	void WindowFactoryManager::addFactory(WindowFactory* factory)
	{
		// assert if passed factory is null.
		assert(factory);

		// assert if type name for factory is already in use.
		if (d_factoryRegistry.find(factory->getTypeName().c_str()) != d_factoryRegistry.end())
		{
			assert(0);
		}

		// add the factory to the registry
		d_factoryRegistry[factory->getTypeName().c_str()] = factory;
	}

	WindowFactory* WindowFactoryManager::getFactory(const std::string& type) const
	{
		WindowFactoryRegistry::const_iterator pos = d_factoryRegistry.find(type);

		// found an actual factory for this type
		if (pos != d_factoryRegistry.end())
		{
			return pos->second;
		}

		return 0;
	}

	void WindowFactoryManager::destroyAllFactory()
	{
		for (WindowFactoryRegistry::iterator i = d_factoryRegistry.begin(); i != d_factoryRegistry.end(); ++i)
		{
			delete i->second;
		}
		d_factoryRegistry.clear();
	}
}