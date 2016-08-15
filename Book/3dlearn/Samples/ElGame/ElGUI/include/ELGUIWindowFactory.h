#ifndef __ELGUIWindowFactory_H__
#define __ELGUIWindowFactory_H__

#include "ELGUIWindow.h"
#include <string>

#define ELGUI_DECLARE_WINDOW_FACTORY(T) \
class T ## Factory : public WindowFactory \
{ \
public: \
	T ## Factory(const std::string& type) : WindowFactory(type) {} \
	Window* createWindow(const std::string& name) \
	{ \
		return new T(d_type, name); \
	} \
	void destroyWindow(Window* window) \
	{ \
		delete window; \
	} \
};

namespace ELGUI
{
	class WindowFactory
	{
	public:
		WindowFactory(const std::string& type) : d_type(type) {}
		virtual ~WindowFactory() {}

		virtual	Window* createWindow(const std::string& name) = 0;
		virtual void destroyWindow(Window* window) = 0;

		const std::string& getTypeName() const { return d_type; }

	protected:
		std::string		d_type;
	};
}

#endif //__ELGUIWindowFactory_H__