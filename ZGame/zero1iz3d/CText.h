// CText.h - 


#ifndef __CTEXT_INCLUDED
#define __CTEXT_INCLUDED


#include "COpenGL.h"
#include <string>

using namespace std;

//////////////////
class CText;
class CText2D;
class CText3D;
//////////////////


struct __texttype
{
	string fontname;
	int    size;
	bool   italic;
	bool   bold;
	bool   underlined;
    float  zv;
};


// base text class
class CText 
{

protected:
	unsigned int base;				// base display for the glGenList

public:

	~CText() { this->Destroy(); };

	virtual void Setup( COpenGL &gl, __texttype tt ) = 0;
	virtual void Write( string str ) = 0;

	void Destroy();

	// predefine operators
	CText& operator<<( int n ) { char buf[8]; sprintf( buf, "%d", n ); this->Write( buf ); return *this; };
	CText& operator<<( unsigned n ) { char buf[8]; sprintf( buf, "%d", n ); this->Write( buf ); return *this; };
	CText& operator<<( float n ) { char buf[16]; sprintf( buf, "%f", n ); this->Write( buf ); return *this; };
	CText& operator<<( char n ) { char buf[2]; sprintf( buf, "%c", n ); this->Write( buf ); return *this; };
	CText& operator<<( string str ) { this->Write( str ); return *this; };
};


// 2D texts
class CText2D : public CText 
{

public:
	void Setup( COpenGL &gl, __texttype tt );
	void Write( string str );

};


// 3D texts
class CText3D : public CText
{

private:
	float width;

public:
	void Setup( COpenGL &gl, __texttype tt );
	void Write ( string str );

};


#endif