#ifndef __ELGUISlotFunctor_H__
#define __ELGUISlotFunctor_H__

#include "ELGUIEventArgs.h"

namespace ELGUI
{
	class SlotFunctorBase
	{
	public:
		virtual ~SlotFunctorBase() {}
		virtual bool operator()(const EventArgs& args) = 0;
	};

	template<typename T>
	class MemberFunctionSlot : public SlotFunctorBase
	{
	public:
		 typedef bool(T::*MemberFunctionType)(const EventArgs&);

		 MemberFunctionSlot(MemberFunctionType func, T* obj)
			 : d_function(func),
			 d_object(obj)
		 {

		 }

		 virtual bool operator() (const EventArgs& args)
		 {
			 return (d_object->*d_function)(args);
		 }

	private:
		MemberFunctionType	d_function;
		T*	d_object;
	};
}

#endif //__ELGUISlotFunctor_H__