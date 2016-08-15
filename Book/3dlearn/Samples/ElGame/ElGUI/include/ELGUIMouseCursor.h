#ifndef __ELGUIMouseCursor_H__
#define __ELGUIMouseCursor_H__

#include "ELGUISingleton.h"
#include "ELGUIVector.h"

namespace ELGUI
{
	class MouseCursor : public Singleton<MouseCursor>
	{
	public:
		MouseCursor();
		virtual ~MouseCursor();

		static MouseCursor& getSingleton();
		static MouseCursor* getSingletonPtr();

		void setPosition(const Point& position);
		void offsetPosition(const Point& offset);
		Point getPosition() const;

		void setSize(const Size& sz);
		Size getSize() const;

	protected:
		Point		d_position;
		Size		d_size;
	};
}

#endif //__ELGUIMouseCursor_H__