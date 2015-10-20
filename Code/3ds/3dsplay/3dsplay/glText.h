#ifndef __GLFONT_32167_H__
#define __GLFONT_32167_H__

#include <Windows.h>
#include <xstring>
#include <map>
using namespace std;

struct FontDesc
{
//列表编号
    int iList;
//字体描述
    GLYPHMETRICSFLOAT flyfloat;
};

class CGlFont
{
protected:
    HFONT m_hFont;
    float m_fExtrusion;
public:
    CGlFont();
    ~CGlFont();

    void DeleteFont();
    bool InitFont(char *fontName);
    bool InitFont(LOGFONT &lf);

    HFONT GetFontHandle()
    {
        return m_hFont;
    }
    LOGFONT GetLogFont();

// Get font bitmap use wglUseFontBitmaps, then draw in the scene
    void Draw2DText(char *string);
// Use wglUseFontOutlines,return the length of the string
    void Draw3DText(const char *string);
    void SetColor(int r, int g, int b);

    float GetExtrusion()
    {
        return m_fExtrusion;
    }
    void SetExtrusion(float extrusion)
    {
        m_fExtrusion = extrusion;
    }
public:
    double m_X;   //字符串位置
    double m_Y;   //字符串位置
	double m_Z;	  //字符串位置

    double m_dHeight; //文字高

    string m_str;  //文字内容

    double m_rotateTheta;//旋转角度

    int m_iDisplayList; //显示列表ID

    static map<UINT,FontDesc>  m_sglFontList;

    int m_red; //红(255色)
    int m_green; //绿
    int m_blue; //蓝
};

#endif    // __GLFONT_32167_H__