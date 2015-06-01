// 3ds_format.h -

// da go smenq s moi, 3ds sux 



#ifndef __3DS_INCLUDED
#define __3DS_INCLUDED





// Primary Chunk -> at the beginning of the 3ds file 
#define PRIMARY       0x4D4D

// Main Chunks
#define OBJECTINFO    0x3D3D				// This gives the version of the mesh and is found right before the material and object information
#define VERSION       0x0002				// .3ds file version
#define EDITKEYFRAME  0xB000				// Key frame data info

// sub defines of OBJECTINFO
#define MATERIAL	  0xAFFF				// This stored the texture info
#define OBJECT		  0x4000				// This stores the faces, vertices, etc...

// sub defines of MATERIAL
#define MATNAME       0xA000				// This holds the material name
#define MATDIFFUSE    0xA020				// This holds the color of the object/material
#define MATMAP        0xA200				// This is a header for a new material
#define MATMAPFILE    0xA300				// This holds the file name of the texture

#define OBJECT_MESH   0x4100				// This lets us know that we are reading a new object

// sub defines of OBJECT_MESH
#define OBJECT_VERTICES     0x4110			// The objects vertices
#define OBJECT_FACES		0x4120			// The objects faces
#define OBJECT_MATERIAL		0x4130			// This is found if the object has a material, either texture map or color
#define OBJECT_UV			0x4140			// The UV texture coordinates

// sub defines of KEYFRAMES


struct _chunk
{
	unsigned short int id;        // chunk ID
	unsigned int len;			  // chunk length
	//unsigned int bytesread;
};



class C3ds_Loader
{

private:


public:

	C3ds_Loader();
	~C3ds_Loader();

	bool Load( const char *3ds_filename );


};





// Once again I should point out that the coordinate system of OpenGL and 3DS Max are different.
// Since 3D Studio Max Models with the Z-Axis pointing up (strange and ugly I know! :), 
// we need to flip the y values with the z values in our vertices.  That way it
// will be normal, with Y pointing up.  Also, because we swap the Y and Z we need to negate 
// the Z to make it come out correctly.  This is also explained and done in ReadVertices().
//
// CHUNKS: What is a chunk anyway?
// 
// "The chunk ID is a unique code which identifies the type of data in this chunk 
// and also may indicate the existence of subordinate chunks. The chunk length indicates 
// the length of following data to be associated with this chunk. Note, this may 
// contain more data than just this chunk. If the length of data is greater than that 
// needed to fill in the information for the chunk, additional subordinate chunks are 
// attached to this chunk immediately following any data needed for this chunk, and 
// should be parsed out. These subordinate chunks may themselves contain subordinate chunks. 
// Unfortunately, there is no indication of the length of data, which is owned by the current 
// chunk, only the total length of data attached to the chunk, which means that the only way 
// to parse out subordinate chunks is to know the exact format of the owning chunk. On the 
// other hand, if a chunk is unknown, the parsing program can skip the entire chunk and 
// subordinate chunks in one jump. " - Jeff Lewis (werewolf@worldgate.com)
//
// In a short amount of words, a chunk is defined this way:
// 2 bytes - Stores the chunk ID (OBJECT, MATERIAL, PRIMARY, etc...)
// 4 bytes - Stores the length of that chunk.  That way you know when that
//           chunk is done and there is a new chunk.
//
// So, to start reading the 3DS file, you read the first 2 bytes of it, then
// the length (using fread()).  It should be the PRIMARY chunk, otherwise it isn't
// a .3DS file.  
//
// Below is a list of the order that you will find the chunks and all the know chunks.
// If you go to www.wosit.org you can find a few documents on the 3DS file format.
// You can also take a look at the 3DS Format.rtf that is included with this tutorial.
//
//
//
//      MAIN3DS  (0x4D4D)
//     |
//     +--EDIT3DS  (0x3D3D)
//     |  |
//     |  +--EDIT_MATERIAL (0xAFFF)
//     |  |  |
//     |  |  +--MAT_NAME01 (0xA000) (See mli Doc) 
//     |  |
//     |  +--EDIT_CONFIG1  (0x0100)
//     |  +--EDIT_CONFIG2  (0x3E3D) 
//     |  +--EDIT_VIEW_P1  (0x7012)
//     |  |  |
//     |  |  +--TOP            (0x0001)
//     |  |  +--BOTTOM         (0x0002)
//     |  |  +--LEFT           (0x0003)
//     |  |  +--RIGHT          (0x0004)
//     |  |  +--FRONT          (0x0005) 
//     |  |  +--BACK           (0x0006)
//     |  |  +--USER           (0x0007)
//     |  |  +--CAMERA         (0xFFFF)
//     |  |  +--LIGHT          (0x0009)
//     |  |  +--DISABLED       (0x0010)  
//     |  |  +--BOGUS          (0x0011)
//     |  |
//     |  +--EDIT_VIEW_P2  (0x7011)
//     |  |  |
//     |  |  +--TOP            (0x0001)
//     |  |  +--BOTTOM         (0x0002)
//     |  |  +--LEFT           (0x0003)
//     |  |  +--RIGHT          (0x0004)
//     |  |  +--FRONT          (0x0005) 
//     |  |  +--BACK           (0x0006)
//     |  |  +--USER           (0x0007)
//     |  |  +--CAMERA         (0xFFFF)
//     |  |  +--LIGHT          (0x0009)
//     |  |  +--DISABLED       (0x0010)  
//     |  |  +--BOGUS          (0x0011)
//     |  |
//     |  +--EDIT_VIEW_P3  (0x7020)
//     |  +--EDIT_VIEW1    (0x7001) 
//     |  +--EDIT_BACKGR   (0x1200) 
//     |  +--EDIT_AMBIENT  (0x2100)
//     |  +--EDIT_OBJECT   (0x4000)
//     |  |  |
//     |  |  +--OBJ_TRIMESH   (0x4100)      
//     |  |  |  |
//     |  |  |  +--TRI_VERTEXL          (0x4110) 
//     |  |  |  +--TRI_VERTEXOPTIONS    (0x4111)
//     |  |  |  +--TRI_MAPPINGCOORS     (0x4140) 
//     |  |  |  +--TRI_MAPPINGSTANDARD  (0x4170)
//     |  |  |  +--TRI_FACEL1           (0x4120)
//     |  |  |  |  |
//     |  |  |  |  +--TRI_SMOOTH            (0x4150)   
//     |  |  |  |  +--TRI_MATERIAL          (0x4130)
//     |  |  |  |
//     |  |  |  +--TRI_LOCAL            (0x4160)
//     |  |  |  +--TRI_VISIBLE          (0x4165)
//     |  |  |
//     |  |  +--OBJ_LIGHT    (0x4600)
//     |  |  |  |
//     |  |  |  +--LIT_OFF              (0x4620)
//     |  |  |  +--LIT_SPOT             (0x4610) 
//     |  |  |  +--LIT_UNKNWN01         (0x465A) 
//     |  |  | 
//     |  |  +--OBJ_CAMERA   (0x4700)
//     |  |  |  |
//     |  |  |  +--CAM_UNKNWN01         (0x4710)
//     |  |  |  +--CAM_UNKNWN02         (0x4720)  
//     |  |  |
//     |  |  +--OBJ_UNKNWN01 (0x4710)
//     |  |  +--OBJ_UNKNWN02 (0x4720)
//     |  |
//     |  +--EDIT_UNKNW01  (0x1100)
//     |  +--EDIT_UNKNW02  (0x1201) 
//     |  +--EDIT_UNKNW03  (0x1300)
//     |  +--EDIT_UNKNW04  (0x1400)
//     |  +--EDIT_UNKNW05  (0x1420)
//     |  +--EDIT_UNKNW06  (0x1450)
//     |  +--EDIT_UNKNW07  (0x1500)
//     |  +--EDIT_UNKNW08  (0x2200)
//     |  +--EDIT_UNKNW09  (0x2201)
//     |  +--EDIT_UNKNW10  (0x2210)
//     |  +--EDIT_UNKNW11  (0x2300)
//     |  +--EDIT_UNKNW12  (0x2302)
//     |  +--EDIT_UNKNW13  (0x2000)
//     |  +--EDIT_UNKNW14  (0xAFFF)
//     |
//     +--KEYF3DS (0xB000)
//        |
//        +--KEYF_UNKNWN01 (0xB00A)
//        +--............. (0x7001) ( viewport, same as editor )
//        +--KEYF_FRAMES   (0xB008)
//        +--KEYF_UNKNWN02 (0xB009)
//        +--KEYF_OBJDES   (0xB002)
//           |
//           +--KEYF_OBJHIERARCH  (0xB010)
//           +--KEYF_OBJDUMMYNAME (0xB011)
//           +--KEYF_OBJUNKNWN01  (0xB013)
//           +--KEYF_OBJUNKNWN02  (0xB014)
//           +--KEYF_OBJUNKNWN03  (0xB015)  
//           +--KEYF_OBJPIVOT     (0xB020)  
//           +--KEYF_OBJUNKNWN04  (0xB021)  
//           +--KEYF_OBJUNKNWN05  (0xB022)  
//
// Once you know how to read chunks, all you have to know is the ID you are looking for
// and what data is stored after that ID.  You need to get the file format for that.
// I can give it to you if you want, or you can go to www.wosit.org for several versions.
// Because this is a proprietary format, it isn't a official document.
//
// I know there was a LOT of information blown over, but it is too much knowledge for
// one tutorial.  In the animation tutorial that I eventually will get to, some of
// the things explained here will be explained in more detail.  I do not claim that
// this is the best .3DS tutorial, or even a GOOD one :)  But it is a good start, and there
// isn't much code out there that is simple when it comes to reading .3DS files.
// So far, this is the best I have seen.  That is why I made it :)
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com


#endif