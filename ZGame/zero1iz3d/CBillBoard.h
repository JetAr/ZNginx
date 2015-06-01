// CBillBoard.h -



#ifndef __CBILLBOARD_INCLUDED
#define __CBILLBOARD_INCLUDED

#include <vector>


// BillBoard 
class CBillBoardFragment
{

protected:

	CVector pos;
	float	width;
	float	height;
	rgba	clr;


public:
	CBillBoardFragment();
	CBillBoardFragment( const CBillBoardFragment &bb );

	void Rotate( CVector axis_angles );
	void setPosition( CVector vpos ) { pos = vpos; };
    void setSize( float w, float h ) { width = w; height = h; };
	void setColor( rgb new_clr ) { clr = new_clr; };
	void setColor( rgba new_clr ) { clr = new_clr; };

	const CVector& getPosition() { return pos; };
	void getSize( float &w, float &h ) { w = width; h = height; };
	const rgba& getColor() { return clr; };

	void operator=( CBillBoardFragment bf ) {};

};



// BillBoard Group
class CBillBoard
{

private:
	typedef vector<CBillBoardFragment> billtype;

	CTexture	tex;		// billboard texture
	billtype	bills;		// billboards

public:

	//CBillBoard();
	~CBillBoard() { Clear(); };
	
	void Add( CBillBoardFragment bill ) { bills.push_back( bill ); };
	void setTexture( CTexture &texture ); 
	int setTexture( string filename );
	void Reneder( RENDERER device );

	void Clear() { bills.clear(); };
	void Destroy();

};




#endif
