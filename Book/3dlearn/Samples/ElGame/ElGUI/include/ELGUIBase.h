#ifndef __ELGUIBase_H__
#define __ELGUIBase_H__

namespace ELGUI
{
static const float DefaultNativeHorzRes	= 640.0f;	// Default native horizontal resolution (for fonts and imagesets)
static const float DefaultNativeVertRes	= 480.0f;	// Default native vertical resolution (for fonts and imagesets)

#define CEGUI_ALIGN_ELEMENTS_TO_PIXELS 1

#if defined(CEGUI_ALIGN_ELEMENTS_TO_PIXELS)
	# define PixelAligned(x) ((float)(int)((x) + ((x) > 0.0f ? 0.5f : -0.5f)))
#else
	# define PixelAligned(x) (x)
#endif
}

#endif //__ELGUIBase_H__