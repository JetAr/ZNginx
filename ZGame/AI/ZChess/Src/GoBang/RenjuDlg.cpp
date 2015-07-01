// RenjuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Renju.h"
#include "RenjuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
extern int count;//���ڹ�ֵ������ȫ�ֱ���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenjuDlg dialog

CRenjuDlg::CRenjuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenjuDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenjuDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRenjuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenjuDlg)
	DDX_Control(pDX, IDC_NODECOUNT, m_OutputInfo);
	DDX_Control(pDX, IDC_THINKPRG, m_ThinkProgress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRenjuDlg, CDialog)
	//{{AFX_MSG_MAP(CRenjuDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTNNEWGAME, OnBtnnewgame)
	ON_BN_CLICKED(IDC_BTNEIXT, OnBtneixt)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenjuDlg message handlers

BOOL CRenjuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	memset(m_RenjuBoard,NOSTONE,GRID_COUNT);  //��ʼ������
	CMoveGenerator* pMG;
	CEveluation* pEvel;
	m_nUserStoneColor=BLACK;
	m_pSE=new CNegaScout_TT_HH;				  //����NegaScout_TT_HH��������
	pMG=new CMoveGenerator;					  //�����߷�������
	pEvel=new CEveluation;					  //������ֵ����
	m_pSE->SetThinkProgress(&m_ThinkProgress);//�趨������
	m_pSE->SetSearchDepth(3);                 //�趨Ĭ���������
	m_pSE->SetMoveGenerator(pMG);			  //�趨�߷�������
	m_pSE->SetEveluator(pEvel);			      //�趨��ֵ����

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRenjuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRenjuDlg::OnPaint() 
{
	CPaintDC dc(this);
	
	//������
	for(int i=0;i<GRID_NUM;i++)
	{
		dc.MoveTo(BOARD_POS_X,BOARD_POS_Y+i*BOARD_WIDTH);
		dc.LineTo(BOARD_POS_X+14*BOARD_WIDTH,BOARD_POS_Y+i*BOARD_WIDTH);
	}
	
	//������
	for(int j=0;j<GRID_NUM;j++)
	{
		dc.MoveTo(BOARD_POS_X+j*BOARD_WIDTH,BOARD_POS_Y);
		dc.LineTo(BOARD_POS_X+j*BOARD_WIDTH,BOARD_POS_Y+14*BOARD_WIDTH);
	}
	
	//������������
/*	char ch[2];
	
	ch[0]='a';
	ch[1]=0;
	dc.SetBkMode(TRANSPARENT);
	for(i=0;i<GRID_NUM;i++)
	{
		dc.TextOut(BOARD_POS_X+i*BOARD_WIDTH-5,BOARD_POS_Y+14*BOARD_WIDTH+20,ch);
		ch[0]+=1;
	}
	
	//������������
	ch[0]='O';
	ch[1]=0;
	for(i=0;i<GRID_NUM;i++)
	{
		dc.TextOut(BOARD_POS_X-BOARD_WIDTH,BOARD_POS_Y+i*BOARD_WIDTH-10,ch);
		ch[0]-=1;
	}
*/
	//����ǵ�
	dc.SelectStockObject(BLACK_BRUSH);
	dc.Ellipse(BOARD_POS_X+3*BOARD_WIDTH-3,BOARD_POS_Y+3*BOARD_WIDTH-3,BOARD_POS_X+3*BOARD_WIDTH+3,BOARD_POS_Y+3*BOARD_WIDTH+3);
	dc.Ellipse(BOARD_POS_X+11*BOARD_WIDTH-3,BOARD_POS_Y+3*BOARD_WIDTH-3,BOARD_POS_X+11*BOARD_WIDTH+3,BOARD_POS_Y+3*BOARD_WIDTH+3);
	dc.Ellipse(BOARD_POS_X+7*BOARD_WIDTH-3,BOARD_POS_Y+7*BOARD_WIDTH-3,BOARD_POS_X+7*BOARD_WIDTH+3,BOARD_POS_Y+7*BOARD_WIDTH+3);
	dc.Ellipse(BOARD_POS_X+3*BOARD_WIDTH-3,BOARD_POS_Y+11*BOARD_WIDTH-3,BOARD_POS_X+3*BOARD_WIDTH+3,BOARD_POS_Y+11*BOARD_WIDTH+3);
	dc.Ellipse(BOARD_POS_X+11*BOARD_WIDTH-3,BOARD_POS_Y+11*BOARD_WIDTH-3,BOARD_POS_X+11*BOARD_WIDTH+3,BOARD_POS_Y+11*BOARD_WIDTH+3);
	
	//������
	int x;int y;

	for(x=0;x<GRID_NUM;x++)
	{
		for(y=0;y<GRID_NUM;y++)
		{
			if(m_RenjuBoard[y][x]==BLACK)
			{
				dc.SelectStockObject(BLACK_BRUSH);
				dc.Ellipse(BOARD_POS_X+x*BOARD_WIDTH-BOARD_WIDTH/2+1,BOARD_POS_Y+y*BOARD_WIDTH-BOARD_WIDTH/2+1,BOARD_POS_X+x*BOARD_WIDTH+BOARD_WIDTH/2-1,BOARD_POS_Y+y*BOARD_WIDTH+BOARD_WIDTH/2-1);
			}
			else 
				if(m_RenjuBoard[y][x]==WHITE)
				{
					dc.SelectStockObject(WHITE_BRUSH);
					dc.Ellipse(BOARD_POS_X+x*BOARD_WIDTH-BOARD_WIDTH/2+1,BOARD_POS_Y+y*BOARD_WIDTH-BOARD_WIDTH/2+1,BOARD_POS_X+x*BOARD_WIDTH+BOARD_WIDTH/2-1,BOARD_POS_Y+y*BOARD_WIDTH+BOARD_WIDTH/2-1);
				}
		}
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRenjuDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRenjuDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDC* pDC;

	pDC=GetDC();
	CRect rect(BOARD_POS_X-5,BOARD_POS_Y-5,BOARD_POS_X+14*BOARD_WIDTH+5,BOARD_POS_Y+14*BOARD_WIDTH+5);
	int i=(point.x+BOARD_WIDTH/2-1-BOARD_POS_X)/BOARD_WIDTH;
	int j=(point.y+BOARD_WIDTH/2-1-BOARD_POS_Y)/BOARD_WIDTH;

	//������µ����� 
	//�����껻��������ϵĸ���
	if(IsValidPos(i,j) && m_RenjuBoard[j][i]==(BYTE)NOSTONE))
	{
		//������Ч����
		m_RenjuBoard[j][i]=m_nUserStoneColor;
		
		switch(m_nUserStoneColor)
		{
			case BLACK:
				pDC->SelectStockObject(BLACK_BRUSH);
				break;
			
			case WHITE:
				pDC->SelectStockObject(WHITE_BRUSH);
				break;
		}
		
		pDC->Ellipse(BOARD_POS_X+i*BOARD_WIDTH-BOARD_WIDTH/2+1,BOARD_POS_Y+j*BOARD_WIDTH-BOARD_WIDTH/2+1,BOARD_POS_X+i*BOARD_WIDTH+BOARD_WIDTH/2-1,BOARD_POS_Y+j*BOARD_WIDTH+BOARD_WIDTH/2-1);//���Ƹ��µ�����
		m_ThinkProgress.SetPos(0);//���ý�����Ϊ0		
		m_OutputInfo.SetWindowText("������˼�����������...");//����ȴ���Ϣ

		int timecount;

		timecount=GetTickCount();//ȡ��ǰʱ��
		count=0;//����ֵ��������
		if(m_nUserStoneColor==BLACK)
			InvertRenjuBroad();//����û�ִ��,�����̷�ת	
		m_pSE->SearchAGoodMove(m_RenjuBoard,BLACK);//����������,������һ����
		if(m_nUserStoneColor==BLACK)
			InvertRenjuBroad();//����û�ִ��,�����̷�ת
		
		CString sNodeCount;
		//���������ʱ�������Ľڵ���
		sNodeCount.Format(" Cost% d ms. % d Nodes were eveluated.",GetTickCount()-timecount,count);
		m_OutputInfo.SetWindowText(sNodeCount);
	}

	InvalidateRect(NULL,TRUE);
	UpdateWindow();
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CRenjuDlg::OnBtnnewgame() 
{
	// TODO: Add your control notification handler code here
	CNewGameDlg NewGame;//���� NewGame�Ի���

	if(NewGame.DoModal()==IDOK)
	{ 
		
		m_pSE->SetSearchDepth(NewGame.GetSelectedPly());//�����������Ϊ�û���ѡ���	
		m_nUserStoneColor=NewGame.GetStoneColor();      //�����û�ѡ���������ɫ
		memset(m_RenjuBoard,NOSTONE,GRID_COUNT);		//��ʼ������
		if(m_nUserStoneColor==WHITE)					//���û�ִ��,����һ����
			m_RenjuBoard[7][7]=BLACK;					//Black First
		InvalidateRect(NULL,TRUE);		
		UpdateWindow();									//�ػ���Ļ
	}
}

//�˺������ڷ�ת�����ϵ�����(�ڱ��,�ױ��)
void CRenjuDlg::InvertRenjuBroad()
{
	int x,y;

	for(x=0;x<GRID_NUM;x++)
	{
		for(y=0;y<GRID_NUM;y++)
		{
			switch(m_RenjuBoard[y][x])
			{
			case WHITE:
				m_RenjuBoard[y][x]=BLACK;
				break;
			
			case BLACK:
				m_RenjuBoard[y][x]=WHITE;
				break;
			
			default:break;
			}
		}
	}
}

void CRenjuDlg::OnBtneixt() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOK);
}

void CRenjuDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	if(m_pSE)
	{
		delete m_pSE;
		m_pSE = NULL;
	}
}
