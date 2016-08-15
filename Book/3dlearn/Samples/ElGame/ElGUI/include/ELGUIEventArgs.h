#ifndef __ELGUIEventArgs_H__
#define __ELGUIEventArgs_H__

namespace ELGUI
{
	class EventArgs
	{
	public:
		EventArgs() : handled(false) {}
		virtual ~EventArgs() {}

		// Data Members
		bool	handled;
	};
}

#endif //__ELGUIEventArgs_H__