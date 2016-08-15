#include "ELGUIPCH.h"
#include "ELGUIEventSet.h"

namespace ELGUI
{
	EventSet::EventSet()
	: d_muted(false)
	{

	}

	EventSet::~EventSet()
	{
		removeAllEvents();
	}

	void EventSet::addEvent(const std::string& name)
	{
		if (isEventPresent(name))
		{
			assert(0 && std::string("An event named '" + name + "' already exists in the EventSet.").c_str());
		}

		d_events[name.c_str()] = new Event(name);
	}
	
	void EventSet::removeEvent(const std::string& name)
	{
		EventMap::iterator pos = d_events.find(name.c_str());

		if (pos != d_events.end())
		{
			delete pos->second;
			d_events.erase(pos);
		}
	}
	
	void EventSet::removeAllEvents()
	{
		EventMap::iterator pos = d_events.begin();
		EventMap::iterator end = d_events.end()	;

		for (; pos != end; ++pos)
		{
			delete pos->second;
		}

		d_events.clear();
	}

	bool EventSet::isEventPresent(const std::string& name)
	{
		return (d_events.find(name.c_str()) != d_events.end());
	}

	bool EventSet::subscribeEvent(const std::string& name, SlotFunctorBase* slot)
	{
		Event* event = getEventObject(name, true);
		if (event)
			return event->subscribe(slot);
		else
			return false;
	}

	void EventSet::fireEvent(const std::string& name, EventArgs& args)
	{
		fireEvent_impl(name, args);
	}

	bool EventSet::isMuted() const
	{
		return d_muted;
	}

	void EventSet::setMutedState(bool setting)
	{
		d_muted = setting;
	}
	
	Event* EventSet::getEventObject(const std::string& name, bool autoAdd /* = false */)
	{
		EventMap::iterator pos = d_events.find(name.c_str());

		// if event did not exist, add it and then find it.
		if (pos == d_events.end())
		{
			if (autoAdd)
			{
				addEvent(name);
				return d_events.find(name.c_str())->second;
			}
			else
			{
				return 0;
			}
		}

		return pos->second;
	}

	void EventSet::fireEvent_impl(const std::string& name, EventArgs& args)
	{
		// find event object
		Event* ev = getEventObject(name);

		// fire the event if present and set is not muted
		if ((ev != 0) && !d_muted)
			(*ev)(args);
	}
}