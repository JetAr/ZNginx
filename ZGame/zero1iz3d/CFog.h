// CFog.h -




#ifndef __CFOG_INCLUDED
#define __CFOG_INCLUDED


#define DEFAULT_DENSITY 0.025f

/*
   Some Technicals
   ----------------------

   end,start - kydeto maglata zapochva i svarshva
   0.0 < z < 1.0 - dalbochina na myglata ot view-pointa
   density - nasitenost na myglata, samo za EXP i EXP2 !

*/


typedef enum
{
    FOG_LINEAR,    // f = ( end - z ) / ( end - start )
    FOG_EXP,	   // f = pow( e, -( density*z ) )
    FOG_EXP2	   // f = pow( e, -( density*z )^2 )
} FOG_MODE;


class CFog
{

private:

    rgb clr;
    float density;
    float startz, endz;
    FOG_MODE mode;

public:

    CFog();
    CFog( FOG_MODE fog_mode, rgb color, float start, float end, float dens = DEFAULT_DENSITY );

    void Initialize( FOG_MODE fog_mode, rgb color, float start, float end, float dens = DEFAULT_DENSITY );
    void setEnable();
    void setDisable();
    void setLOD( bool bLOD );

};

#endif
