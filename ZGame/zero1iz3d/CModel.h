// CModel.h -



#ifndef __CMODEL_INCLUDED
#define __CMODEL_INCLUDED

/*
  ierarhiq na 3DS

  | MODEL
  |-> | FRAME(S)
      |->  MESH(ES)
	       MATERIAL(S) 
	     
  
*/


/////////////////////////
class CMaterial;
class CMesh;
class CModelFrame;
class CModel;
/////////////////////////


// This is our face structure.  This is is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	BYTE  color[3];				// The color of the object (R, G, B)
	int   texureId;				// the texture ID
	float uTile;				// u tiling of texture  (Currently not used)
	float vTile;				// v tiling of texture	(Currently not used)
	float uOffset;			    // u offset of texture	(Currently not used)
	float vOffset;				// v offset of texture	(Currently not used)
};

// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject				// 1 MESH
{
	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object
	CVector  *pVerts;			// The object's vertices
	CVector  *pNormals;		// The object's normals
	CVector2D  *pTexVerts;		// The texture's UV coordinates
	tFace *pFaces;				// The faces information of the object
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
/*struct t3DModel 
{
	int numOfObjects;					// The number of objects in the model
	int numOfMaterials;					// The number of materials for the model
	vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	vector<t3DObject> pObject;			// The object list for our model
};*/


// Material Class
class CMaterial
{

public:
	string  strName[255];			// texture name
	string  strFile[255];			// filename na texturata ( ako ima takyv znachi imame texture map )
	BYTE    color[3];				// cvqt na obekta (R, G, B)
	int     texureId;				// texture ID
	float   us;						// u scale
	float   vs;						// v scale
	float   uoff;					// u offset of texture	(Currently not used)
	float   voff;					// v offset of texture	(Currently not used)
};

// Mesh Class
class CMesh
{

private:

	struct _Face
	{
		int idxVert[3];		 	    // indeksi na tochkite systavqshti triygalnika
		int idxCoord[3];			// indeksi na koordinatite na texturata koito texturirat tozi triagalnik
	};


	int num_vertices;				// broi tochki v modela
	CVector *pvertices;				// tochki na obekta
	CVector *pnormals;				// normali na obekta

	int num_faces;					// broi lica v modela
	_Face *pfaces;					// lica na obekta


	int num_texvertices;			// broi na koordinatite na texturite
	CVector2D *ptexvertices;		// UV koordinati na texturata

	int matID;						// nomer na materiala (texturata)
	bool bTextured;					// texturiran li e
	std::string strName;			// ime na obekta


public:

	CMesh();
	~CMesh();

	void Destroy();

	int getMaterialID() { return matID; };

};


// Frame Class
class CModelFrame
{

private:


	bool bLoaded;

public:

	int num_meshes;					// broi obekti v kadyra/obekta
	vector<CMesh> pMeshes;			// list ot obektite v modela/kadyra ni

	int num_materials;				// broi materiali v kadyra/obekta
	vector<CMaterial> pMats;		// list ot materialite (Textures and colors)
	

	CModelFrame() { bLoaded = false; };
	~CModelFrame();
	
	void ComputeNormals();
	void Render();
	void Destroy();
	
};





/***********************************************/
class CModel// : public CLoad3DS
{

private:

	bool bLoaded;

public:

/*	int num_frames;
	CModelFrame *frames;
	*/

	int numOfObjects;					// The number of objects in the model
	int numOfMaterials;					// The number of materials for the model
	vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	vector<t3DObject> pObject;			// The object list for our model


	CModel() { bLoaded = false; };
	
	void ComputeNormals();
	void Render();
	void Destroy();
	

};


#endif