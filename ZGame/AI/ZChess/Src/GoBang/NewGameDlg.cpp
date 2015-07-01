// NewGameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "renju.h"
#include "NewGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewGameDlg dialog


CNewGameDlg::CNewGameDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNewGameDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNewGameDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CNewGameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNewGameDlg)
    DDX_Control(pDX, IDC_PLY, m_SetPly);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewGameDlg, CDialog)
    //{{AFX_MSG_MAP(CNewGameDlg)
    ON_BN_CLICKED(IDC_BLACKSTONE, OnBlackstone)
    ON_BN_CLICKED(IDC_WHITESTONE, OnWhitestone)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewGameDlg message handlers

void CNewGameDlg::OnOK()
{
    // TODO: Add extra validation here
    m_nSelectedPly=m_SetPly.GetPos();//�����û�ѡ����������

    CDialog::OnOK();
}

BOOL CNewGameDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    ((CButton*)GetDlgItem(IDC_BLACKSTONE))->SetCheck(TRUE);
    m_nStoneColor=BLACK;   //�趨Ĭ��������ɫ
    m_SetPly.SetRange(1,5);//�趨������ȷ�Χ
    m_SetPly.SetPos(3);    //Ĭ�ϵ��������Ϊ 3

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewGameDlg::OnBlackstone()
{
    // TODO: Add your control notification handler code here
    m_nStoneColor=BLACK;//�趨������ɫΪ��ɫ
}

void CNewGameDlg::OnWhitestone()
{
    // TODO: Add your control notification handler code here
    m_nStoneColor=WHITE;//�趨������ɫΪ��ɫ
}
