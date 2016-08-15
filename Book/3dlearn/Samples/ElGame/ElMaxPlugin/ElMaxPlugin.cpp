/**********************************************************************
 *<
	FILE: ElMaxPlugin.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "ElMaxPlugin.h"

#define ElMaxPlugin_CLASS_ID	Class_ID(0x8e6134bd, 0x7a151e9c)

static ElMaxPlugin theElMaxPlugin;

//--- ElMaxPluginDesc -------------------------------------------------------
class ElMaxPluginClassDesc : public ClassDesc2
{
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theElMaxPlugin; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return ElMaxPlugin_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ElMaxPlugin"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};

static ElMaxPluginClassDesc ElMaxPluginDesc;
ClassDesc2* GetElMaxPluginDesc() { return &ElMaxPluginDesc; }

//--- ElMaxPluginDlg -------------------------------------------------------
bool GetSaveFileName(HWND hwnd, LPSTR filename, int len, LPSTR filter, LPSTR defExt ,LPSTR title)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME); 
	ofn.hwndOwner = GetActiveWindow(); 
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = filename; 
	ofn.nMaxFile = len;
	ofn.nFilterIndex = 1;
	ofn.lpstrDefExt = defExt;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 

	// Display the Open dialog box.
	if (GetSaveFileName(&ofn))
		return true;

	return false;
}

static INT_PTR CALLBACK ElMaxPluginDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			theElMaxPlugin.Init(hWnd);
			
			Interval animRange;
			ISpinnerControl* spin;
			int frameStart, frameEnd;

			// Setup the spinner control for the static frame#
			// We take the frame 0 as the default value
			animRange = theElMaxPlugin.GetInterface()->GetAnimRange();
			frameStart = animRange.Start() / GetTicksPerFrame();
			frameEnd = animRange.End() / GetTicksPerFrame();
			spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_STATIC_FRAME), EDITTYPE_INT); 
			spin->SetLimits(frameStart, frameEnd, TRUE);
			spin->SetScale(1.0f);
			spin->SetValue(theElMaxPlugin.GetStaticFrame(), FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner controls for the mesh animation start frame
			spin = GetISpinner(GetDlgItem(hWnd, IDC_MESHANIM_START_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd, IDC_MESHANIM_START), EDITTYPE_INT); 
			spin->SetLimits(frameStart, frameEnd, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(frameStart, FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner controls for the mesh animation end frame
			spin = GetISpinner(GetDlgItem(hWnd, IDC_MESHANIM_END_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd, IDC_MESHANIM_END), EDITTYPE_INT); 
			spin->SetLimits(frameStart, frameEnd, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(frameEnd, FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner controls for the mesh animation key sample rate 
			spin = GetISpinner(GetDlgItem(hWnd, IDC_MESHANIM_STEP_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd, IDC_MESHANIM_STEP), EDITTYPE_INT); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(theElMaxPlugin.GetMeshAnimFrameStep(), FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner controls for the skeletal animation start frame
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SKELETALANIM_START_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd, IDC_SKELETALANIM_START), EDITTYPE_INT); 
			spin->SetLimits(frameStart, frameEnd, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(frameStart, FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner controls for the skeletal animation end frame
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SKELETALANIM_END_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd, IDC_SKELETALANIM_END), EDITTYPE_INT); 
			spin->SetLimits(frameStart, frameEnd, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(frameEnd, FALSE);
			ReleaseISpinner(spin);

			// Setup the spinner controls for the skeletal animation key sample rate 
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SKELETALANIM_STEP_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd, IDC_SKELETALANIM_STEP), EDITTYPE_INT); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(theElMaxPlugin.GetSkeletalAnimFrameStep(), FALSE);
			ReleaseISpinner(spin);
		}
		break;

	case WM_DESTROY:
		theElMaxPlugin.Destroy(hWnd);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EXPORT_MESH:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				char filename[MAX_PATH] = {0};
				if (GetSaveFileName(hWnd, filename, sizeof(filename), _T("ElMesh(*.ELM)\0*.ELM\0All(*.*)\0*.*\0"), _T("ELM"), _T("Mesh file to save")))
				{
					ISpinnerControl* spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN));
					theElMaxPlugin.SetStaticFrame(spin->GetIVal());
					ReleaseISpinner(spin);

					theElMaxPlugin.ExportMesh(filename);
				}
			}
			break;
		case IDC_EXPORT_MESHANIM:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				char filename[MAX_PATH] = {0};
				if (GetSaveFileName(hWnd, filename, sizeof(filename), _T("ElMeshAnimation(*.EMA)\0*.EMA\0All(*.*)\0*.*\0"), _T("EMA"), _T("MeshAnimation file to save")))
				{
					ISpinnerControl* spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN));
					theElMaxPlugin.SetStaticFrame(spin->GetIVal());
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd, IDC_MESHANIM_START_SPIN));
					theElMaxPlugin.SetMeshAnimFrameStart(spin->GetIVal());
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd, IDC_MESHANIM_END_SPIN));
					theElMaxPlugin.SetMeshAnimFrameEnd(spin->GetIVal());
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd, IDC_MESHANIM_STEP_SPIN));
					theElMaxPlugin.SetMeshAnimFrameStep(spin->GetIVal());
					ReleaseISpinner(spin);

					theElMaxPlugin.ExportMeshAnim(filename);
				}
			}
			break;
		case IDC_EXPORT_SKELETON:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				char filename[MAX_PATH] = {0};
				if (GetSaveFileName(hWnd, filename, sizeof(filename), _T("ElSkeleton(*.ELS)\0*.ELS\0All(*.*)\0*.*\0"), _T("ELS"), _T("Skeleton file to save")))
				{
					ISpinnerControl* spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN));
					theElMaxPlugin.SetStaticFrame(spin->GetIVal());
					ReleaseISpinner(spin);

					theElMaxPlugin.ExportSkeleton(filename);
				}
			}
			break;
		case IDC_EXPORT_SKELETALANIM:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				char filename[MAX_PATH] = {0};
				if (GetSaveFileName(hWnd, filename, sizeof(filename), _T("ElSkeletalAnimation(*.ESA)\0*.ESA\0All(*.*)\0*.*\0"), _T("ESA"), _T("SkeletalAnimation file to save")))
				{
					ISpinnerControl* spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN));
					theElMaxPlugin.SetStaticFrame(spin->GetIVal());
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd, IDC_SKELETALANIM_START_SPIN));
					theElMaxPlugin.SetSkeletalAnimFrameStart(spin->GetIVal());
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd, IDC_SKELETALANIM_END_SPIN));
					theElMaxPlugin.SetSkeletalAnimFrameEnd(spin->GetIVal());
					ReleaseISpinner(spin);

					spin = GetISpinner(GetDlgItem(hWnd, IDC_SKELETALANIM_STEP_SPIN));
					theElMaxPlugin.SetSkeletalAnimFrameStep(spin->GetIVal());
					ReleaseISpinner(spin);

					theElMaxPlugin.ExportSkeletalAnim(filename);
				}
			}
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		theElMaxPlugin.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
		break;

	default:
		return 0;
	}
	return 1;
}

//--- ElMaxPlugin -------------------------------------------------------
ElMaxPlugin::ElMaxPlugin()
{
	nStaticFrame = 0;
	nMeshAnimStartFrame = 0;
	nMeshAnimEndFrame = 100;
	nMeshAnimKeyFrameStep = 5;
	nSkeletalAnimStartFrame = 0;
	nSkeletalAnimEndFrame = 100;
	nSkeletalAnimKeyFrameStep = 1;

	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

ElMaxPlugin::~ElMaxPlugin()
{

}

void ElMaxPlugin::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		ElMaxPluginDlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void ElMaxPlugin::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void ElMaxPlugin::Init(HWND hWnd)
{

}

void ElMaxPlugin::Destroy(HWND hWnd)
{

}

bool ElMaxPlugin::ExportMesh(LPCTSTR filename)
{
	// Open the stream
	outfile.open(filename);
	if (!outfile.good())
		return false;

	// Set format of the stream
	outfile << setprecision(4) << setiosflags(ios::fixed);

	// Add bones to out bone list, in case some bone be
	// exported as GeomObject
	boneList.Clear();
	PreProcess(ip->GetRootNode());

	// Add the nodes material to out material list
	mtlList.Clear();
	FindMaterials(ip->GetRootNode());

	// Export global info
	ExportGlobalInfo();

	// Export list of material definitions
	ExportMaterialList();

	// Export each object found in the scene
	nodeEnum(ip->GetRootNode());

	// Close the stream
	outfile.close();

	return true;
}

bool ElMaxPlugin::ExportMeshAnim(LPCTSTR filename)
{
	// Open the stream
	outfile.open(filename);
	if (!outfile.good())
		return false;

	// Set format of the stream
	outfile << setprecision(4) << setiosflags(ios::fixed);

	// Add bones to out bone list, in case some bone be
	// exported as GeomObject
	boneList.Clear();
	PreProcess(ip->GetRootNode());

	// Export global info
	ExportGlobalInfo();

	// Export animation of each object found in the scene
	nodeEnumForAnim(ip->GetRootNode());

	// Close the stream
	outfile.close();

	return true;
}

bool ElMaxPlugin::ExportSkeleton(LPCTSTR filename)
{
	// Open the stream
	outfile.open(filename);
	if (!outfile.good())
		return false;

	// Set format of the stream
	outfile << setprecision(4) << setiosflags(ios::fixed);

	// Add bones to out bone list
	boneList.Clear();
	PreProcess(ip->GetRootNode());

	// Export global info
	ExportGlobalInfo();

	// Export list of bone
	ExportBoneList();

	// Close the stream
	outfile.close();

	return true;
}

bool ElMaxPlugin::ExportSkeletalAnim(LPCTSTR filename)
{
	// Open the stream
	outfile.open(filename);
	if (!outfile.good())
		return false;

	// Set format of the stream
	outfile << setprecision(4) << setiosflags(ios::fixed);

	// Export global info
	ExportGlobalInfo();

	// Add bones to out bone list
	boneList.Clear();
	PreProcess(ip->GetRootNode());

	// Export list of animation
	ExportSkeletalAnim();

	// Close the stream
	outfile.close();

	return true;
}

void ElMaxPlugin::ExportGlobalInfo()
{
	outfile << indent.str() << "ElMaxPlugin" << endl;
	++indent;
	outfile << indent.str() << "Version " << 100 << endl;
	--indent;
}

