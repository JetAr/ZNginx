#include "ELGUIPCH.h"
#include "ELGUIEvent.h"

namespace ELGUI
{
	Event::Event(const std::string& name)
	: d_name(name)
	{

	}

	Event::~Event()
	{
		SlotContainer::iterator iter(d_slots.begin());
		SlotContainer::iterator end_iter(d_slots.end());

		for (; iter != end_iter; ++iter)
		{
			delete *iter;
		}
		d_slots.clear();
	}

	bool Event::subscribe(SlotFunctorBase* slot)
	{
		d_slots.push_back(slot);
		return true;
	}

	const std::string& Event::getName() const
	{
		return d_name;
	}

	void Event::operator()(EventArgs& args)
	{
		SlotContainer::iterator iter(d_slots.begin());
		SlotContainer::iterator end_iter(d_slots.end());

		// execute all subscribers, updating the 'handled' state as we go
		for (; iter != end_iter; ++iter)
		{
			args.handled |= (*(*iter))(args);
		}
	}
}