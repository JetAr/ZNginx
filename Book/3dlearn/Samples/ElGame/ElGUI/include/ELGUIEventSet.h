#ifndef __ELGUIEventSet_H__
#define __ELGUIEventSet_H__

#include "ELGUIEvent.h"
#include <map>

namespace ELGUI
{
	class EventSet
	{
	public:
		EventSet();
		virtual ~EventSet();

		void addEvent(const std::string& name);
		void removeEvent(const std::string& name);
		void removeAllEvents();
		bool isEventPresent(const std::string& name);

		virtual bool subscribeEvent(const std::string& name, SlotFunctorBase* slot);
		virtual void fireEvent(const std::string& name, EventArgs& args);

		bool isMuted() const;
		void setMutedState(bool setting);

	protected:
		Event* getEventObject(const std::string& name, bool autoAdd = false);
		void fireEvent_impl(const std::string& name, EventArgs& args);

	protected:
		typedef std::map<std::string, Event*>	EventMap;
		EventMap	d_events;

		bool	d_muted;
	};
}

#endif //__ELGUIEventSet_H__