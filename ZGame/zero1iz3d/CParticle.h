// CParticle.h -



#ifndef __CPARTICLE_INCLUDED
#define __CPARTICLE_INCLUDED

#include <list>
#include "Geometry.h"


/////////////////////
class CParticle;
class CParticleCore;
/////////////////////


#define MAX_PARTICLES  100
#define PARTICLE_LIST  list<CParticle>
#define PARTICLE_ITER  list<CParticle>::iterator


// particle unit
class CParticle
{

public:

    CSprite3D sprite3;

    // position and velocity
    CVector  pos;			// particle position
    CVector	 vel;			// particle velocity
    CVector  grav;			// particle gravity
    CVector  cur_speed;		// current speed

    int  	 dir_angle;		// velocity angle
    int  	 spread_angle;	// particles spread angle

    float	 r_speed;
    float	 t_speed;

    // life vars
    float	 life;			// life
    float	 lifespan;		// lifespan
    bool	 alive;

    // color vars
    rgba     clr;
    float	 clrspan;		// color fade speed


    CParticle();
    CParticle( const CParticle &particle );
    void Clear();

};


// particle system
class CParticleCore
{

protected:

    CVector	   org;				// warp origin
    float      distLOD;			// LOD distance

    CVector	   min_gravity;		// minimum gravity
    CVector	   max_gravity;		// maximum gravity
    CVector	   wind;			// wind factor

    float	   min_life;		// minimum particle life
    float	   max_life;		// maximum particle life

    bool	   loop;			// particle animation loop

    int		   count;
    PARTICLE_LIST units;


public:

    CParticleCore();
    ~CParticleCore();

    virtual void Initialize() {};
    virtual void Update( RENDERER device, float mpf ) = 0;
    //virtual void Respawn( PARTICLE_ITER particle ) = 0; // regenerates particle
    void Destroy();

};


// particle systems

class CParticleRandom : public CParticleCore
{

public:

    void Initialize( CVector origin, int particle_count = MAX_PARTICLES );
    void Update( RENDERER device, float mpf );
    void Respawn( PARTICLE_ITER &particle );

};



#endif
