#ifndef __ELGUIEvent_H__
#define __ELGUIEvent_H__

#include "ELGUISlotFunctor.h"
#include <string>
#include <vector>

namespace ELGUI
{
	class Event
	{
	public:
		Event(const std::string& name);
		virtual ~Event();

		bool subscribe(SlotFunctorBase* slot);
		const std::string& getName() const;
		void operator()(EventArgs& args);

	protected:
		typedef std::vector<SlotFunctorBase*>	SlotContainer;
		SlotContainer	d_slots;

		const std::string	d_name;
	};
}

#endif //__ELGUIEvent_H__