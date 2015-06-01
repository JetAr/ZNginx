// CSprite.h -


#ifndef __CSPRITE_INCLUDED
#define __CSPRITE_INCLUDED

#include <vector>


////////////////////
class CSprite;
class CSprite2D;
class CSprite3D;
////////////////////


struct __frame
{
	float width;
	float height;
	CTexture texture;
};



// base sprite class
class CSprite
{

protected:
	rgba clr;				  // sprite color and alpha value
	ALPHABLENDING AlphaBlend; // enable/disable alphablending
	int num_frames;			  // total frames
	int cur_frame;			  // current frame to render
	bool bRotate;			  // is sprite rotating
	bool bAnimated;			  // is sprite animated
	vector<__frame> frames;   // texture animation frames
	
public:

	CSprite();
	~CSprite();

	int Load( string filename );
	//int AddFrame();

	void setAlpha( ALPHABLENDING Alpha ) { AlphaBlend = Alpha; };
	ALPHABLENDING getAlpha() { return AlphaBlend; };
	void setColor( rgba new_clr ) { clr = new_clr; };
	void setColor( rgb new_clr ) { clr = new_clr; };
	void setRotate( bool bRot_val ) { bRotate = bRot_val; };
	bool getRotate() { return bRotate; };
	void setSize( float w, float h );
	float getWidth() { return frames[cur_frame].width; };
	float getHeight() { return frames[cur_frame].height; };

	virtual void Render( RENDERER device ) = 0;

};


// 2D ortho sprite
class CSprite2D : public CSprite
{

private:
	CVector2D   pos;		    // position
	float       rot_speed;  	// rotation-speed
	float	    rot_angle;		// rotation angle

public:

	CSprite2D();

	void Render( RENDERER device );
	void Rotate( float new_angle ) { rot_angle = new_angle; };	
	void setPosition( CVector2D new_pos ) { pos = new_pos; };
	void setRotationSpeed( float new_rot_speed ) { rot_speed = new_rot_speed; };
};


// 3D sprite
class CSprite3D : public CSprite
{

private:
	CVector		pos;			// xyz position
	CVector		rot_speed;		// xyz rotation speed
	CVector		rot_angle;		// xyz rotation angles

public:
	
	CSprite3D();

	void Render( RENDERER device );
	void Rotate( CVector new_angle ) { rot_angle = new_angle; };
	void setPosition( CVector new_pos ) { pos = new_pos; };
	void setRotationSpeed( CVector new_rot_speed ) { rot_speed = new_rot_speed; };

};


#endif
