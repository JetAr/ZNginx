#ifndef __ElGUITrueTypeFont_H__
#define __ElGUITrueTypeFont_H__

#include "ELGUIFont.h"
#include "ELGUIImageset.h"
#include "ElD3DHeaders.h"

class ElTrueTypeFont : public ELGUI::Font
{
public:
	ElTrueTypeFont(const std::string& face_name, const std::string& name, int size = 16, bool bold = false, bool italic = false, bool antialias = true);
	~ElTrueTypeFont();

	virtual void load();
	virtual void rasterize(wchar_t codepoint);

protected:
	void _assignNewGlyphImageset();

protected:
	unsigned char			d_num_imageset_used;
	ELGUI::Imageset*		d_current_imageset;
	IDirect3DTexture9*		d_current_texture;

	int						d_x_bottom;
	int						d_y_bottom;

	std::string				d_name;
	std::string				d_face_name;
	HFONT					d_font;

	const int				d_size;
	const bool				d_bold;
	const bool				d_italic;
	const bool				d_antialias;
};

#endif //__ElGUITrueTypeFont_H__