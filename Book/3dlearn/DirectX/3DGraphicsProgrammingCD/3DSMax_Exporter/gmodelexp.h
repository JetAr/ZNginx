//************************************************************************** 
//* gmodelexp.h	- Gmodel Exporter
//* 
//* By Peter Pon
//*
//* Dec 30, 2007
//***************************************************************************

#ifndef _GMODELEXP_
#define _GMODELEXP_

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "exportmesh.h"

#include <vector>

extern ClassDesc* GetGmodelExpDesc();
extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#define VERSION			200			// Version number * 100

#define MAX_TEXTURES 4
// fixed pipeline material

enum eTextureType
{
	TEXTURE_DIFFUSE,
	TEXTURE_LIGHTMAP,
	TEXTURE_ENVIRONMENT
};

struct sMaterial
{
	Color m_EmissiveColor;
	Color m_AmbientColor;
	Color m_DiffuseColor;
	Color m_SpecularColor;
	float m_fShininess;
	bool  m_bCullFace;

	int m_iNumTextures;
	int m_MapChannel[MAX_TEXTURES];
	std::string m_Textures[MAX_TEXTURES];

	std::string m_BlendMode;

	sMaterial(void)
	{
		m_fShininess = 0.0f;
		m_iNumTextures = 0;
		m_bCullFace = true;

		for ( int i=0; i<MAX_TEXTURES; i++ )
		{
			m_MapChannel[i] = 0;
		}
	}

	bool ConvertMTL(Mtl *mtl);
};

// Exporter Kernel
class GmodelExp : public SceneExport 
{
public:
	GmodelExp();
	~GmodelExp();

	// SceneExport methods
	int    ExtCount();     // Number of extensions supported 
	const TCHAR * Ext(int n);     // Extension #n (i.e. "ASC")
	const TCHAR * LongDesc();     // Long ASCII description (i.e. "Ascii Export") 
	const TCHAR * ShortDesc();    // Short ASCII description (i.e. "Ascii")
	const TCHAR * AuthorName();    // ASCII Author name
	const TCHAR * CopyrightMessage();   // ASCII Copyright message 
	const TCHAR * OtherMessage1();   // Other message #1
	const TCHAR * OtherMessage2();   // Other message #2
	unsigned int Version();     // Version number * 100 (i.e. v3.01 = 301) 
	void	ShowAbout(HWND hWnd);  // Show DLL's "About..." box
	int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file
	BOOL	SupportsOptions(int ext, DWORD options);

	// Other methods
	// Node enumeration
	void	TraceTree(INode* node);
	void	BuildTable(void);
	bool	NodeFilter(INode *node);
	void	AddNode(INode *node);

	int		SearchMaterial(Mtl *mtl);
	void	AddMaterial(Mtl *mtl);

	//
	void	ExportHeader(void);
	void	ExportMaterial(void);
	void	ExportNodeMesh(INode *pNode, int index);
	void	ExportMesh(void);

	//
	void	PushSpace(void);
	void	PopSpace(void);
	void	PrintSpace(void);

	inline Interface*	GetInterface()		{ return ip; }

public:

	static BOOL	m_bExportSelectedNodesOnly;
	static BOOL	m_bExportMesh;
	static BOOL	m_bExportMaterial;
	static BOOL	m_bExportShaderMaterial;
	static BOOL	m_bExportVertexNormals;
	static BOOL	m_bExportVertexColors;
	static BOOL	m_bExportVertexUVs;
	static BOOL m_bExportTangentSpace;

	BOOL	m_bTriangleStrip;
	BOOL	m_bObjectSpace;

	TimeValue m_EvaluateTime;

	std::vector<INode *> m_NodeTable;
	int			m_iNumNodes;

	std::vector<Mtl *> m_MaterialTable;
	int			m_iNumMaterials;

	int			m_iSpaceLevel;
	char		m_szSpace[64];

	FILE*		m_pFile;
	Interface*	ip;
	CMeshBuilder m_MeshBuilder;
};

#endif // _GMODELEXP_

