// 3ds_format.cpp -


#include "main.h"




C3ds_Loader::C3ds_Loader()
{


}


C3ds_Loader::~C3ds_Loader()
{


}



bool C3ds_Loader::Load( const char *3ds_filename )
{

	FILE     *fp;
	_chunk   chunk;


	// open file
	if ( ( fp = fopen( 3ds_filename, "r" ) ) == NULL )
		return false;

	// read PRIMARY chunk
	//...

	// read chunks
	while( !feof( fp ) )
	{

		fread( &chunk, 1, sizeof(_chunk), fp );

	//	printf( 


	}

}