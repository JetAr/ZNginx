// CParticle.cpp -


#include "main.h"



////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CParticle()
//// Desc: class constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CParticle::CParticle()
{
	lifespan = clrspan = life = 0.0;
	alive = false;

	//sprite3.Load( "data/star.tga" );
	//sprite3.setSize( 0.2f, 0.2f );

}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CParticle()
//// Desc: class constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CParticle::CParticle( const CParticle &particle )
{
/*	pos = particle.pos;
	acc = particle.acc;
	grav = particle.grav;
	speed = particle.speed;
	life = particle.life;
	fade = particle.fade;
	alive = particle.alive;*/
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Clear()
//// Desc: 
////////////////////////////////////////////////////////////////////////////////////////////////
void CParticle::Clear()
{

	lifespan = clrspan = life = 0.0;
	alive = false;

	sprite3.Load( "data/star.tga" );
	sprite3.setSize( 0.2f, 0.2f );
	sprite3.setRotate( true );
	sprite3.setRotationSpeed( CVector( 0.0f, 0.0f, 10.0f ) );

}




////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CParticleCore()
//// Desc: class constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CParticleCore::CParticleCore()
{
	count = 0;
	loop = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: ~CParticleCore()
//// Desc: class destructor
////////////////////////////////////////////////////////////////////////////////////////////////
CParticleCore::~CParticleCore()
{


}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Destroy()
//// Desc: free memory
////////////////////////////////////////////////////////////////////////////////////////////////
void CParticleCore::Destroy()
{

	/*
	if ( units != NULL )
		delete[] units;
		*/
	
	units.clear();
	count = 0;
	loop = false;
}


/*
void CParticleCore::Initialize(CVector origin, CVector gravity, CVector fairwind, int particle_count, bool particle_loop )
{

	org = origin;
	grav = gravity;
	wind = fairwind;
	count = particle_count;
	loop = particle_loop;

	units = new CParticle[count];

	sprite3.Load( "myfile.tga" );
}


*/


//-------------------------- Particle Systems --------------------


void CParticleRandom::Initialize( CVector origin, int particle_count )
{

	// setup position
	org = origin;
	min_gravity = CVector( 0.0f, -9.7f, 0.0f );
	max_gravity = CVector( 0.0f, -9.8f, 0.0f );
	wind = CVector( 0.0f, 0.0f, 0.0f );

	min_life = 250.0f;
	max_life = 400.0f;

	loop = true;
	count = particle_count;

	// setup particles	
	CParticle particle;
	particle.Clear();
	units.insert( units.begin(), count, particle );

	list<CParticle>::iterator p = units.begin();
	for ( ; p != units.end(); p++ )
	{
		p->Clear();
		Respawn( p );
	}

}



void CParticleRandom::Respawn( PARTICLE_ITER &particle )
{

	CVector   dir;

	// setup new particle
	dir.x = 0.02f; //rndf(2.2f);// - 0.1f;
	dir.y = 0.02f; //rndf(2.08f);
	dir.z = 0;//rndf(0.02f);// - 0.01f;

	// setup position
	particle->pos = org;
	particle->vel = dir;
	// setup velocity
	particle->cur_speed = dir; //CVector( 0.0f, 0.0f, 0.0f );
	particle->r_speed = rndf( PI * 0.05f );
	particle->t_speed = rndf( PI * 0.25f );
	particle->dir_angle = 360;
	particle->spread_angle = 0;
	// setup forces
	particle->grav.x = 0.0f; //rndf(max_gravity.x - min_gravity.x) + min_gravity.x;
	particle->grav.y = rndf(max_gravity.y - min_gravity.y) + min_gravity.y;
	particle->grav.z = 0.0f; //rndf(max_gravity.z - min_gravity.z) + min_gravity.z;
	// setup lifespan
	particle->life = rndf(max_life - min_life) + min_life;
	particle->lifespan = 1.2f;
	particle->alive = true;
	// setup colors
	particle->clr = rgba( 1.0f ,1.0f, 1.0f, 1.0f );
	particle->clrspan = 0.2f;

}



void CParticleRandom::Update( RENDERER device, float mpf )
{

	PARTICLE_ITER p = units.begin();

	for ( ; p != units.end(); p++ )
	{
		if ( p->alive )
		{
	
			/*sprite3.setRotationSpeed( CVector( 0.0f, 0.0f, 1.0f ) );
			sprite3.setRotate( true );*/

			//p->vel += ((p->grav * 0.5f) * (mpf * mpf) ); 
			//p->cur_speed += (p->vel * mpf) + ((p->grav * 0.5f) * (mpf * mpf) );
			//p->pos += p->vel * mpf;
			//p->pos.y += (p->grav.y * 0.5f) * (mpf * mpf);
			//p->vel.y = p->vel.y + p->grav.y * mpf;
			
			p->cur_speed.x += g_cos[p->dir_angle] * p->vel.x * mpf; 
			p->cur_speed.y += (g_sin[p->dir_angle] * p->vel.y * mpf) + (p->grav.y * 0.5f) * (mpf * mpf); 
			p->cur_speed.z += p->vel.z * mpf;
			p->pos += p->cur_speed;
			
			// fade color
			p->clr.a -= p->clrspan * mpf;
	
			// fade life
			p->life -= p->lifespan;
		
			if ( p->life < 0.0f )
				p->alive = false;

			// SRC_ALPHA, ONE -> additive blending
			p->sprite3.setPosition( p->pos );
			p->sprite3.setAlpha( ALPHABLEND_ADDITIVE );
			p->sprite3.setColor( p->clr );
			p->sprite3.Render( device );
			
			/*

			glBegin(GL_QUADS);
			glVertex2f( p->pos.x, p->pos.y );
			glVertex2f( p->pos.x+0.2f, p->pos.y );
			glVertex2f( p->pos.x+0.2f, p->pos.y+0.2f );
			glVertex2f( p->pos.x, p->pos.y+0.2f );
			glEnd();*/
		}
		else
		{
			Respawn( p );
		}

	}


}


