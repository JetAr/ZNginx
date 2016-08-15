//************************************************************************** 
//* gmodelexp.cpp - Gmodel Exporter
//* 
//* By Peter Pon
//*
//* Dec 30 2007
//*
//***************************************************************************

#include "gmodelexp.h"
#include "3dsmaxport.h"

// Class ID. 
#define GMODELEXP_CLASS_ID	Class_ID(0x6c9521b2, 0x364d3c19)

static BOOL showPrompts;
static BOOL exportSelected;

class GmodelExpClassDesc : public ClassDesc 
{
public:
	int				IsPublic() { return 1; }
	void*			Create(BOOL loading = FALSE) { return new GmodelExp; } 
	const TCHAR*	ClassName() { return GetString(IDS_ASCIIEXP); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; } 
	Class_ID		ClassID() { return GMODELEXP_CLASS_ID; }
	const TCHAR*	Category() { return GetString(IDS_CATEGORY); }
};

static GmodelExpClassDesc GmodelExpDesc;

ClassDesc* GetGmodelExpDesc()
{
	return &GmodelExpDesc;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
}

bool sMaterial::ConvertMTL(Mtl *mtl)
{
	char filename[64];
	char file_ext[16];
	char filename_with_ext[128];

	m_EmissiveColor = mtl->GetSelfIllumColor();
	m_AmbientColor = mtl->GetAmbient();
	m_DiffuseColor = mtl->GetDiffuse();
	m_SpecularColor = mtl->GetSpecular();
	m_fShininess = mtl->GetShininess();

	m_iNumTextures = 0;

	m_BlendMode = "replace";

	if ( mtl->ClassID()==Class_ID(DMTL_CLASS_ID, 0) ) 
	{
		StdMat* std = (StdMat*)mtl;

		float fOpacity = std->GetOpacity(0);
		if ( fOpacity < 1.0f )
		{
			switch (std->GetTransparencyType()) 
			{
			case TRANSP_FILTER: 
				m_BlendMode = "blend";
				break;
			case TRANSP_SUBTRACTIVE: 
				m_BlendMode = "subtract";
				break;
			case TRANSP_ADDITIVE: 
				m_BlendMode = "add";
				break;
			default: 
				m_BlendMode = "replace";
				break;
			}	
		}

		m_bCullFace = !std->GetTwoSided();
	}

	for (int i=0; i<mtl->NumSubTexmaps(); i++) 
	{
		Texmap *tex = mtl->GetSubTexmap(i);
		if ( tex && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00) ) 
		{
			bool valid_channel = false;
			int texture_type = -1;

			switch(i)
			{
			case 0: // ambientmap/lightmap
				texture_type = TEXTURE_LIGHTMAP;
				break;
			case 1: // diffusemap
				texture_type = TEXTURE_DIFFUSE;
				break;
			case 9: // environment
				texture_type = TEXTURE_ENVIRONMENT;
				break;
			default:
				// not supported by fixed pipeline 3D rendering
				break;
			}

			if ( texture_type >= 0 )
			{
				TSTR mapName = ((BitmapTex *)tex)->GetMapName();
				_splitpath(mapName, NULL, NULL, filename, file_ext);
				sprintf(filename_with_ext, "%s%s", filename, file_ext);
				m_Textures[texture_type] = filename_with_ext;
				m_MapChannel[texture_type] = tex->GetMapChannel()-1;
			}
		}
	}

	return true;
}

BOOL GmodelExp::m_bExportSelectedNodesOnly = FALSE;
BOOL GmodelExp::m_bExportMesh = TRUE;
BOOL GmodelExp::m_bExportMaterial = TRUE;
BOOL GmodelExp::m_bExportShaderMaterial = FALSE;
BOOL GmodelExp::m_bExportVertexNormals = TRUE;
BOOL GmodelExp::m_bExportVertexColors = FALSE;
BOOL GmodelExp::m_bExportVertexUVs = TRUE;
BOOL GmodelExp::m_bExportTangentSpace = FALSE;

GmodelExp::GmodelExp()
{
	m_bObjectSpace = TRUE;
	m_EvaluateTime = 0;

	m_iSpaceLevel = 0;
	m_szSpace[0] = '\0';
}

GmodelExp::~GmodelExp()
{
}

int GmodelExp::ExtCount()
{
	return 2;
}

const TCHAR * GmodelExp::Ext(int n)
{
	switch(n) 
	{
	case 0:
		return _T("gma");
	case 1:
		return _T("gm");
	}
	return _T("");
}

const TCHAR * GmodelExp::LongDesc()
{
	return GetString(IDS_LONGDESC);
}

const TCHAR * GmodelExp::ShortDesc()
{
	return GetString(IDS_SHORTDESC);
}

const TCHAR * GmodelExp::AuthorName() 
{
	return _T("Peter Pon");
}

const TCHAR * GmodelExp::CopyrightMessage() 
{
	return GetString(IDS_COPYRIGHT);
}

const TCHAR * GmodelExp::OtherMessage1() 
{
	return _T("");
}

const TCHAR * GmodelExp::OtherMessage2() 
{
	return _T("");
}

unsigned int GmodelExp::Version()
{
	return 100;
}

static INT_PTR CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		CenterWindow(hWnd, GetParent(hWnd)); 
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}       

void GmodelExp::ShowAbout(HWND hWnd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

// Dialog proc
static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg,WPARAM wParam, LPARAM lParam)
{
	GmodelExp *exp = DLGetWindowLongPtr<GmodelExp*>(hWnd); 

	switch (msg) 
	{
	case WM_INITDIALOG:
		exp = (GmodelExp*)lParam;
		DLSetWindowLongPtr(hWnd, lParam); 
		CenterWindow(hWnd, GetParent(hWnd));

		CheckDlgButton(hWnd, IDC_MATERIAL, exp->m_bExportMaterial);

		CheckDlgButton(hWnd, IDC_MESHDATA, exp->m_bExportMesh);
		CheckDlgButton(hWnd, IDC_NORMALS,  exp->m_bExportVertexNormals); 
		CheckDlgButton(hWnd, IDC_TEXCOORDS,exp->m_bExportVertexUVs); 
		CheckDlgButton(hWnd, IDC_VERTEXCOLORS,exp->m_bExportVertexColors); 
		CheckDlgButton(hWnd, IDC_TANGENTSPACE,exp->m_bExportTangentSpace); 

		// Enable / disable mesh options
		EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), exp->m_bExportMesh);
		EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), exp->m_bExportMesh);
		EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), exp->m_bExportMesh);
		EnableWindow(GetDlgItem(hWnd, IDC_TANGENTSPACE), exp->m_bExportMesh);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		case IDC_MESHDATA:
			// Enable / disable mesh options
			EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), IsDlgButtonChecked(hWnd,
				IDC_MESHDATA));
			EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), IsDlgButtonChecked(hWnd,
				IDC_MESHDATA));
			EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), IsDlgButtonChecked(hWnd,
				IDC_MESHDATA));
			EnableWindow(GetDlgItem(hWnd, IDC_TANGENTSPACE), IsDlgButtonChecked(hWnd,
				IDC_MESHDATA));
			break;

		case IDOK:
			exp->m_bExportMesh = IsDlgButtonChecked(hWnd, IDC_MESHDATA); 
			exp->m_bExportMaterial = IsDlgButtonChecked(hWnd, IDC_MATERIAL); 
			exp->m_bExportVertexNormals = IsDlgButtonChecked(hWnd, IDC_NORMALS);
			exp->m_bExportVertexUVs = IsDlgButtonChecked(hWnd, IDC_TEXCOORDS); 
			exp->m_bExportVertexColors = IsDlgButtonChecked(hWnd, IDC_VERTEXCOLORS); 
			exp->m_bExportTangentSpace = IsDlgButtonChecked(hWnd, IDC_TANGENTSPACE); 

			EndDialog(hWnd, 1);
			break;

		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}       

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}

// 輸出檔案
int GmodelExp::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
{
	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	// Grab the interface pointer.
	ip = i;

	if(showPrompts)
	{
		// Prompt the user with our dialogbox, and get all the options.
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_GMODELEXPORT_DLG),
			ip->GetMAXHWnd(), ExportDlgProc, (LPARAM)this)) 
		{
			return 1;
		}
	}

	// Open the stream
	m_pFile = _tfopen(name,_T("wt"));
	if ( !m_pFile ) 
	{
		return 0;
	}

	// Startup the progress bar.
	ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, fn, NULL);

	// Get node & material list
	BuildTable();

	// Export file header
	ExportHeader();

	// Material
	//if ( m_bExportMaterial )
	ExportMaterial();

	// Mesh
	//if ( m_bExportMesh )
	ExportMesh();

	// Done :) Finish the progress bar.
	ip->ProgressEnd();

	// Close the stream
	fclose(m_pFile);

	return 1;
}

BOOL GmodelExp::SupportsOptions(int ext, DWORD options) 
{
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

bool GmodelExp::NodeFilter(INode *node)
{
	if(!exportSelected || node->Selected()) 
	{
		ObjectState os = node->EvalWorldState(m_EvaluateTime); 
		if ( os.obj && 
			os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID && 
			os.obj->ConvertToType(0, triObjectClassID) )
		{
			return true;
		}
	}

	return false;
}

void GmodelExp::AddNode(INode *node)
{
	m_iNumNodes++;
	m_NodeTable.push_back(node);
}

void GmodelExp::AddMaterial(Mtl *mtl)
{
	int material_index = SearchMaterial(mtl);
	if ( material_index!=-1 )
		return;

	if ( mtl->IsMultiMtl() )
	{
		int num_mtls = mtl->NumSubMtls();
		for ( int m=0; m<num_mtls; m++ )
		{
			Mtl *submtl = mtl->GetSubMtl(m);
			m_MaterialTable.push_back(submtl);
		}
	}
	else
	{
		m_MaterialTable.push_back(mtl);
	}

	m_iNumMaterials = m_MaterialTable.size();
}

void GmodelExp::TraceTree(INode* node)
{
	if ( NodeFilter(node) )
	{
		AddNode(node);
	}

	for (int c = 0; c < node->NumberOfChildren(); c++) 
	{
		TraceTree(node->GetChildNode(c));
	}
}

int GmodelExp::SearchMaterial(Mtl *mtl)
{
	for ( int i=0; i<m_iNumMaterials; i++ )
	{
		if ( m_MaterialTable[i]==mtl )
			return i;
	}

	return -1;
}

void GmodelExp::BuildTable(void)
{
	m_iNumNodes = m_iNumMaterials = 0;
	TraceTree(ip->GetRootNode());

	if ( m_bExportMaterial )
	{
		for ( int i=0; i<m_iNumNodes; i++ )
		{
			INode *pNode = m_NodeTable[i];
			Mtl *mtl = pNode->GetMtl();
			if ( mtl )
				AddMaterial(mtl);
		}
	}
}

void GmodelExp::ExportHeader(void)
{
	fprintf(m_pFile, "Gmodel V1.00\n");
}

void GmodelExp::ExportMaterial(void)
{
	PrintSpace(); fprintf(m_pFile, "Begin Material\n");
	PrintSpace(); fprintf(m_pFile, "{\n");
	PushSpace();

	m_iNumMaterials = m_MaterialTable.size();

	PrintSpace(); fprintf(m_pFile, "Materials %d\n", m_iNumMaterials);

	for ( int i=0; i<m_iNumMaterials; i++ )
	{
		PrintSpace(); fprintf(m_pFile, "{	// material %d\n", i);
		PushSpace();

		Mtl *mtl = m_MaterialTable[i];
		sMaterial dxMaterial;
		dxMaterial.ConvertMTL(mtl);

		const char *name = mtl->GetName();

		PrintSpace(); fprintf(m_pFile, "name = \"%s\"\n", name ? name : "unknown");

		PrintSpace(); fprintf(m_pFile, "emissive = %f,%f,%f\n", dxMaterial.m_EmissiveColor.r, dxMaterial.m_EmissiveColor.g, dxMaterial.m_EmissiveColor.b);
		PrintSpace(); fprintf(m_pFile, "ambient = %f,%f,%f\n", dxMaterial.m_AmbientColor.r, dxMaterial.m_AmbientColor.g, dxMaterial.m_AmbientColor.b);
		PrintSpace(); fprintf(m_pFile, "diffuse = %f,%f,%f\n", dxMaterial.m_DiffuseColor.r, dxMaterial.m_DiffuseColor.g, dxMaterial.m_DiffuseColor.b);
		PrintSpace(); fprintf(m_pFile, "specular = %f,%f,%f\n", dxMaterial.m_SpecularColor.r, dxMaterial.m_SpecularColor.g, dxMaterial.m_SpecularColor.b);
		PrintSpace(); fprintf(m_pFile, "shininess = %f\n", dxMaterial.m_fShininess);

		PrintSpace(); fprintf(m_pFile, "blendmode = %s\n", dxMaterial.m_BlendMode.c_str());
		PrintSpace(); fprintf(m_pFile, "CullFace = %s\n", dxMaterial.m_bCullFace ? "on" : "off");

		PrintSpace(); fprintf(m_pFile, "diffuseMap = \"%s\" MapChannel = %d\n", dxMaterial.m_Textures[0].length() ? dxMaterial.m_Textures[0].c_str() : "None", dxMaterial.m_MapChannel[0]);
		PrintSpace(); fprintf(m_pFile, "lightMap = \"%s\" MapChannel = %d\n", dxMaterial.m_Textures[1].length() ? dxMaterial.m_Textures[1].c_str() : "None", dxMaterial.m_MapChannel[1]);
		PrintSpace(); fprintf(m_pFile, "environmentMap = \"%s\"\n", dxMaterial.m_Textures[2].length() ? dxMaterial.m_Textures[2].c_str() : "None");

		PopSpace();
		PrintSpace(); fprintf(m_pFile, "}\n");
	}

	PopSpace();
	PrintSpace(); fprintf(m_pFile, "}\n");
	PrintSpace(); fprintf(m_pFile, "End Material\n");
}

void GmodelExp::ExportNodeMesh(INode *pNode, int index)
{
	if ( !m_MeshBuilder.BuildMesh(pNode, this) )
	{
		PrintSpace(); fprintf(m_pFile, "{	// not a mesh %d\n", index);
		PushSpace();

		PrintSpace(); fprintf(m_pFile, "name = \"%s\"\n", pNode->GetName());

		PopSpace();
		PrintSpace(); fprintf(m_pFile, "}\n");

		return;
	}

	PrintSpace(); fprintf(m_pFile, "{	// mesh %d\n", index);
	PushSpace();

	PrintSpace(); fprintf(m_pFile, "matrix\n");
	PrintSpace(); fprintf(m_pFile, "{\n");
	PushSpace();

	for ( int r=0; r<4; r++ )
	{
		Point3 *pRow = &m_MeshBuilder.m_Matrix.GetRow(r);
		PrintSpace(); fprintf(m_pFile, "%f,%f,%f\n", pRow->x, pRow->y, pRow->z);
	}

	PopSpace();
	PrintSpace(); fprintf(m_pFile, "}\n");

	PrintSpace(); fprintf(m_pFile, "buffers %d\n", m_MeshBuilder.m_iNumVertexArrays);

	for ( int i=0; i<m_MeshBuilder.m_iNumVertexArrays; i++ )
	{
		PrintSpace(); fprintf(m_pFile, "{\n"); 
		PushSpace();

		sMeshVertexArray *pVertexArray = &m_MeshBuilder.m_VertexArray[i];

		// export vertex array
		int num_vertices = pVertexArray->m_VertexArray.size();

		char szVertexFmt[32];

		strcpy(szVertexFmt, "v");

		if ( m_bExportVertexNormals )
		{
			strcat(szVertexFmt, "_n");
		}

		if ( m_bExportVertexColors )
		{
			strcat(szVertexFmt, "_c");
		}

		if ( m_bExportVertexUVs && m_MeshBuilder.m_iNumUVs )
		{
			strcat(szVertexFmt, "_t");

			int strLen = strlen(szVertexFmt);
			szVertexFmt[strLen] = (char) ('0' + m_MeshBuilder.m_iNumUVs);
			szVertexFmt[strLen+1] = '\0';
		}

		if ( m_bExportTangentSpace && m_bExportVertexUVs && m_MeshBuilder.m_iNumUVs )
		{
			strcat(szVertexFmt, "_p_q");
		}

		PrintSpace(); fprintf(m_pFile, "vertices %d\n", num_vertices);
		PrintSpace(); fprintf(m_pFile, "format %s\n", szVertexFmt);
		PrintSpace(); fprintf(m_pFile, "{\n");
		PushSpace();

		for ( int v=0; v<num_vertices; v++)
		{
			Point3 *pPosition = &pVertexArray->m_VertexArray[v].m_Position;
			Point3 *pNormal = &pVertexArray->m_VertexArray[v].m_Normal;
			Point4 *pColor = &pVertexArray->m_VertexArray[v].m_Color;

			PrintSpace(); 

			fprintf(m_pFile, "vertex %f,%f,%f", pPosition->x, pPosition->y, pPosition->z);

			if ( m_bExportVertexNormals )
				fprintf(m_pFile, " normal %f,%f,%f", pNormal->x, pNormal->y, pNormal->z);

			if ( m_bExportVertexColors )
				fprintf(m_pFile, " color %f,%f,%f,%f", pColor->x, pColor->y, pColor->z, pColor->w);

			if ( m_bExportVertexUVs && m_MeshBuilder.m_iNumUVs )
			{
				for ( int t=0; t<m_MeshBuilder.m_iNumUVs; t++ )
				{
					Point3 *pUV = &pVertexArray->m_VertexArray[v].m_UV[t];
					fprintf(m_pFile, " uv%d %f,%f", t, pUV->x, pUV->y);
				}
			}

			if ( m_bExportTangentSpace && m_bExportVertexUVs && m_MeshBuilder.m_iNumUVs )
			{
				Point3 *p = &pVertexArray->m_VertexArray[v].m_Tangent;
				Point3 *q = &pVertexArray->m_VertexArray[v].m_BiNormal;

				fprintf(m_pFile, " tangent %f,%f,%f", p->x, p->y, p->z);
				fprintf(m_pFile, " binormal %f,%f,%f", q->x, q->y, q->z);
			}

			fprintf(m_pFile, "\n");
		}

		PopSpace();
		PrintSpace(); fprintf(m_pFile, "}\n");

		// export index array
		int num_indices = pVertexArray->m_IndexArray_TriList.size();

		PrintSpace(); fprintf(m_pFile, "triangle_list_indices %d\n", num_indices);
		PrintSpace(); fprintf(m_pFile, "{\n");
		PushSpace();

		int num_faces = num_indices / 3;
		for ( int f=0; f<num_faces; f++)
		{
			unsigned short *pIndex = &pVertexArray->m_IndexArray_TriList[f*3];
			PrintSpace(); fprintf(m_pFile, "%d %d %d\n", pIndex[0], pIndex[1], pIndex[2]);
		}

		PopSpace();
		PrintSpace(); fprintf(m_pFile, "}\n");

		// export batches
		int num_batches = pVertexArray->m_BatchList.size();

		PrintSpace(); fprintf(m_pFile, "batches %d\n", num_batches);
		PrintSpace(); fprintf(m_pFile, "{\n");
		PushSpace();

		for ( int b=0; b<num_batches; b++ )
		{
			sMeshBatch *pBatch = &pVertexArray->m_BatchList[b];

			PrintSpace(); fprintf(m_pFile, "material %d\n", pBatch->m_iMaterialIndex);
			PrintSpace(); fprintf(m_pFile, "faces %d\n", pBatch->m_iNumPrimitives_TriList);
			PrintSpace(); fprintf(m_pFile, "index_begin %d\n", pBatch->m_iIndexArrayBegin_TriList);
		}

		PopSpace();
		PrintSpace(); fprintf(m_pFile, "}\n");

		PopSpace();
		PrintSpace(); fprintf(m_pFile, "}\n");
	}

	PopSpace();
	PrintSpace(); fprintf(m_pFile, "}\n");
}

void GmodelExp::ExportMesh(void)
{
	PrintSpace(); fprintf(m_pFile, "Begin Mesh\n");
	PrintSpace(); fprintf(m_pFile, "{\n");
	PushSpace();

	PrintSpace(); fprintf(m_pFile, "meshes %d\n", m_iNumNodes);

	for ( int i=0; i<m_iNumNodes; i++ )
	{
		INode *node = m_NodeTable[i];
		ExportNodeMesh(node, i);
	}

	PopSpace();
	PrintSpace(); fprintf(m_pFile, "}\n");
	PrintSpace(); fprintf(m_pFile, "End Mesh\n");
}

void GmodelExp::PrintSpace(void)
{
	fprintf(m_pFile, "%s", m_szSpace);
}

void GmodelExp::PushSpace(void)
{
	m_iSpaceLevel++;
	strcat(m_szSpace, "\t");
}

void GmodelExp::PopSpace(void)
{
	if ( m_iSpaceLevel==0 )
		return;
	m_szSpace[--m_iSpaceLevel] = '\0';
}
