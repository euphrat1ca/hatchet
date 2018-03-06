#include "StdAfx.h"
#include "CaptionLeftPanel.h"
#include "resource.h"
#include "MainHatchet.h"

CCaptionLeftPanel::CCaptionLeftPanel(void)
{
}

CCaptionLeftPanel::~CCaptionLeftPanel(void)
{
}
BEGIN_MESSAGE_MAP(CCaptionLeftPanel, CPanel)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

int CCaptionLeftPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPanel::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_imgCaption.IsNull()) return 0;

	m_imgCaption.LoadFromResource(AfxGetInstanceHandle(), IDB_CAPTION);

	return 0;
}

void CCaptionLeftPanel::OnPaint()
{
	if(m_imgCaption.IsNull()) return;

	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);

	m_imgCaption.Draw(dc.m_hDC, rcClient);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		HICON hIcon = (HICON)(DWORD_PTR)GetClassLong(pMainHatchet->m_hWnd,GCL_HICONSM);
		if (hIcon)
		{
			DrawIconEx(dc.m_hDC, 4, 4, hIcon, 16, 16, 0, 0, DI_NORMAL);
		}
	}
}

void CCaptionLeftPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		nShowTab = -1;					//当前显示的TAB，对比，不用每次都变，放到全局，因为点击左边，没有改变
		pMainHatchet->SelchangeTab();
		pMainHatchet->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}

	CPanel::OnLButtonDown(nFlags, point);
}

void CCaptionLeftPanel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		pMainHatchet->SendMessage(WM_NCLBUTTONDBLCLK, HTCAPTION, 0);
	}
	CPanel::OnLButtonDblClk(nFlags, point);
}
