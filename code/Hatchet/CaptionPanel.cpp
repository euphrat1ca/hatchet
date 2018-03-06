#include "StdAfx.h"
#include "CaptionPanel.h"
#include "MainHatchet.h"

CCaptionPanel::CCaptionPanel(void)
{
	m_CaptionLeftPanel.m_hWnd = NULL;
	m_CaptionCenterPanel.m_hWnd = NULL;
	m_CaptionRightPanel.m_hWnd = NULL;
}

CCaptionPanel::~CCaptionPanel(void)
{

}
BEGIN_MESSAGE_MAP(CCaptionPanel, CPanel)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int CCaptionPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPanel::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_CaptionLeftPanel.m_hWnd) return 0;
	if (m_CaptionCenterPanel.m_hWnd) return 0;
	if (m_CaptionRightPanel.m_hWnd) return 0;

	m_CaptionLeftPanel.Create(_T("CaptionLeftPanel"), WS_CHILD|WS_VISIBLE, CRect(0, 0, 30, lpCreateStruct->cy), this, 10001);
	m_CaptionCenterPanel.Create(_T("CaptionCenterPanel"), WS_CHILD|WS_VISIBLE, CRect(30, 0, lpCreateStruct->cx-30-92, lpCreateStruct->cy), this, 10002);
	m_CaptionRightPanel.Create(_T("CaptionRightPanel"), WS_CHILD|WS_VISIBLE, CRect(lpCreateStruct->cx-92, 0, 92, lpCreateStruct->cy), this, 10003);

	return 0;
}

void CCaptionPanel::OnSize(UINT nType, int cx, int cy)
{
	CPanel::OnSize(nType, cx, cy);

	if (m_CaptionLeftPanel.m_hWnd == NULL) return;
	if (m_CaptionCenterPanel.m_hWnd == NULL) return;
	if (m_CaptionRightPanel.m_hWnd == NULL) return;

	CRect rcClient;
	GetClientRect(rcClient);

	m_CaptionLeftPanel.MoveWindow(0, 0, 30, rcClient.Height());
	m_CaptionCenterPanel.MoveWindow(30, 0, rcClient.Width()-30-92, rcClient.Height());
	m_CaptionRightPanel.MoveWindow(rcClient.Width()-92, 0, 92, rcClient.Height());//75

}

void CCaptionPanel::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	if (pMainHatchet)
	{
		CMenu* pSysMenu = pMainHatchet->GetSystemMenu(0);
		if (pSysMenu)
		{
			UpdateSystemMenu(pSysMenu);
			int nID = pSysMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, pMainHatchet);
			if (nID > 0)
			{
				if (nID == SC_CLOSE)
				{
					pMainHatchet->SendMessage(WM_SYSCOMMAND, nID, 0);
					return;
				}
				pMainHatchet->SendMessage(WM_SYSCOMMAND, nID, 0);
			}
		}
	}
}

void CCaptionPanel::UpdateSystemMenu(CMenu* pMenu)
{
	if (AfxGetMainWnd()->IsIconic())
	{
		return;
	}

	if (AfxGetMainWnd()->IsZoomed())
	{
		CString szMenuStr;

		pMenu->GetMenuString(SC_RESTORE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_RESTORE, MF_STRING | MF_ENABLED, SC_RESTORE, szMenuStr);
/*
		pMenu->GetMenuString(SC_MINIMIZE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_MINIMIZE, MF_STRING | MF_ENABLED, SC_MINIMIZE, szMenuStr);
*/
		pMenu->GetMenuString(SC_MAXIMIZE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_MAXIMIZE, MF_STRING | MF_GRAYED, SC_MAXIMIZE, szMenuStr);

		pMenu->GetMenuString(SC_MOVE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_MOVE, MF_STRING | MF_GRAYED, SC_MOVE, szMenuStr);

		pMenu->GetMenuString(SC_SIZE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_SIZE, MF_STRING | MF_GRAYED, SC_SIZE, szMenuStr);
	}
	else
	{
		CString szMenuStr;
		pMenu->GetMenuString(SC_RESTORE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_RESTORE, MF_STRING | MF_GRAYED, SC_RESTORE, szMenuStr);
/*
		pMenu->GetMenuString(SC_MINIMIZE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_MINIMIZE, MF_STRING | MF_ENABLED, SC_MINIMIZE, szMenuStr);

		pMenu->GetMenuString(SC_MAXIMIZE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_MAXIMIZE, MF_STRING | MF_ENABLED, SC_MAXIMIZE, szMenuStr);

		pMenu->GetMenuString(SC_MOVE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_MOVE, MF_STRING | MF_ENABLED, SC_MOVE, szMenuStr);

		pMenu->GetMenuString(SC_SIZE,szMenuStr,MF_BYCOMMAND);
		pMenu->ModifyMenu(SC_SIZE, MF_STRING | MF_ENABLED, SC_SIZE, szMenuStr);
*/
	}

	return;
}