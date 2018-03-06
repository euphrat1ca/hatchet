#include "StdAfx.h"
#include "MainHatchet.h"
#include "CaptionRightPanel.h"
#include "resource.h"

#define IDC_TRAYBTN		1001
#define IDC_MINBTN		1002
#define IDC_MAXBTN		1003
#define IDC_CLOSEBTN	1004

CCaptionRightPanel::CCaptionRightPanel(void)
{
	m_imgTrayBtn.m_hWnd = NULL;
	m_imgMinBtn.m_hWnd = NULL;
	m_imgMaxBtn.m_hWnd = NULL;
	m_imgCloseBtn.m_hWnd = NULL;
}

CCaptionRightPanel::~CCaptionRightPanel(void)
{
}
BEGIN_MESSAGE_MAP(CCaptionRightPanel, CPanel)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TRAYBTN, OnClickedTrayBtn)
	ON_BN_CLICKED(IDC_MINBTN, OnClickedMinBtn)
	ON_BN_CLICKED(IDC_MAXBTN, OnClickedMaxBtn)
	ON_BN_CLICKED(IDC_CLOSEBTN, OnClickedCloseBtn)
END_MESSAGE_MAP()

int CCaptionRightPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPanel::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_imgCaption.IsNull()) return 0;
	if (m_imgTrayBtn.m_hWnd) return 0;
	if (m_imgMinBtn.m_hWnd) return 0;
	if (m_imgMaxBtn.m_hWnd) return 0;
	if (m_imgCloseBtn.m_hWnd) return 0;


	m_imgCaption.LoadFromResource(AfxGetInstanceHandle(), IDB_CAPTION);

	m_imgTrayBtn.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(0,0,23,21), this, IDC_TRAYBTN);
	m_imgTrayBtn.SetImage(IDB_TRAYBTN);

	m_imgMinBtn.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(23,0,46,21), this, IDC_MINBTN);
	m_imgMinBtn.SetImage(IDB_MINBTN);

	m_imgMaxBtn.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(46,0,69,21), this, IDC_MAXBTN);
	m_imgMaxBtn.SetImage(IDB_MAXBTN);

	m_imgCloseBtn.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(69,0,92,21), this, IDC_CLOSEBTN);
	m_imgCloseBtn.SetImage(IDB_CLOSEBTN);


	return 0;
}

void CCaptionRightPanel::OnPaint()
{
	if(m_imgCaption.IsNull()) return;

	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);

	m_imgCaption.Draw(dc.m_hDC, rcClient);
}

void CCaptionRightPanel::OnClickedTrayBtn()							//托盘
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		pMainHatchet->OnMyTray();
	}
}

void CCaptionRightPanel::OnClickedMinBtn()							//最小化
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		pMainHatchet->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
}
void CCaptionRightPanel::OnClickedMaxBtn()							//最大化
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		if (pMainHatchet->IsZoomed())
		{
			AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		}else{
			AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
	}
}
void CCaptionRightPanel::OnClickedCloseBtn()							//关闭
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		pMainHatchet->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}