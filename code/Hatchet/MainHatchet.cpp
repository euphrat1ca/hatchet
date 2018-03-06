// MainHatchet.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "MainHatchet.h"
#include "CaptionCenterPanel.h"
#include "MainDlg.h"
#include "FileDlg.h"
#include "CmdDlg.h"
#include "DataDlg.h"
#include "FileSaveDlg.h"
#include "ProxyDlg.h"
#include "EvalCodeDlg.h"
#include "RegistryDlg.h"
#include "BrowseDlg.h"


#define WM_TRAY		WM_USER+1001	//托盘消息
// CMainHatchet
CMainDlg* pMaindlg;


IMPLEMENT_DYNAMIC(CMainHatchet, CWnd)

CMainHatchet::CMainHatchet()
{
	nOldSelectPos = 0;				//之前选择的标签，记忆选择。文件管理，文件内容管理，内容关闭，回到文件管理
	nNewSelectPos = 0;				//之前选择的标签，记忆选择。文件管理，文件内容管理，内容关闭，回到文件管理
	m_captionPanel.m_hWnd = NULL;
	m_PtrAllDlg.RemoveAll();
}

CMainHatchet::~CMainHatchet()
{
}


BEGIN_MESSAGE_MAP(CMainHatchet, CWnd)
	ON_WM_GETMINMAXINFO()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCACTIVATE()
	ON_MESSAGE(WM_TRAY, OnNotifyIcon)//托盘
END_MESSAGE_MAP()



// CMainHatchet 消息处理程序
void CMainHatchet::OnMyTray()
{
	ShowWindow(SW_HIDE);

	NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	NotifyIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	NotifyIcon.hWnd = m_hWnd;
	lstrcpy(NotifyIcon.szTip, "Hatchet");
	NotifyIcon.uCallbackMessage = WM_TRAY;
	NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &NotifyIcon);

	return;
}

LRESULT CMainHatchet::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	if ((lParam == WM_LBUTTONDOWN))
	{
		Shell_NotifyIcon(NIM_DELETE,&NotifyIcon);	//清除托盘图标
		ModifyStyleEx(0, WS_EX_TOPMOST);
		ShowWindow(SW_SHOW);
		//Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////

BOOL CMainHatchet::PreCreateWindow(CREATESTRUCT& cs)
{
	HINSTANCE hInstance = (HINSTANCE)AfxGetInstanceHandle();
	ASSERT(hInstance);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	BOOL bRet = GetClassInfoEx(hInstance, cs.lpszClass, &wcex);
	if (bRet)
	{
		return TRUE;
	}

	wcex.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = AfxWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = wcex.hIconSm = (HICON)AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = cs.lpszClass;
	bRet = RegisterClassEx(&wcex);
	return bRet;
}

void CMainHatchet::PostNcDestroy()
{
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				delete[] pCDialog;
				pCDialog = NULL;
			}
		}
	}
	delete this;
	delete pMaindlg;

	Shell_NotifyIcon(NIM_DELETE,&NotifyIcon);	//清除托盘图标

	CWnd::PostNcDestroy();
}

void CMainHatchet::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 1024;
	lpMMI->ptMinTrackSize.y = 560;
	lpMMI->ptMaxPosition.x = 0;
	lpMMI->ptMaxPosition.y = 0;

	CRect rcWorkArea;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	lpMMI->ptMaxSize.x = lpMMI->ptMaxTrackSize.x = rcWorkArea.Width();
	lpMMI->ptMaxSize.y = lpMMI->ptMaxTrackSize.y = rcWorkArea.Height();

	CWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainHatchet::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	int nCxBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
	int nCyBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);

	lpncsp->rgrc[0].left -= nCxBorder;
	lpncsp->rgrc[0].right += nCxBorder;
	lpncsp->rgrc[0].top -= nCyBorder;
	lpncsp->rgrc[0].bottom += nCyBorder;

	lpncsp->rgrc[0].left += 2;
	lpncsp->rgrc[0].right -= 2;
	lpncsp->rgrc[0].top += 2;
	lpncsp->rgrc[0].bottom -= 2;

	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CMainHatchet::OnNcPaint()
{
	if (m_imgBorder.IsNull()) return;

	CWindowDC dc(this);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	m_imgBorder.Draw(dc.m_hDC, 0, 0, rcWnd.Width(), 2);
	m_imgBorder.Draw(dc.m_hDC, 0, 0, 2, rcWnd.Height());
	m_imgBorder.Draw(dc.m_hDC, rcWnd.Width()-2, 0, 2, rcWnd.Height());
	m_imgBorder.Draw(dc.m_hDC, 0, rcWnd.Height()-2, rcWnd.Width(), 2);
}

int CMainHatchet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//WIN7
	HINSTANCE hInst = LoadLibrary(_T("Uxtheme.dll"));
	if (hInst)
	{
		typedef HRESULT (WINAPI *PFUN_SetWindowTheme)(HWND, LPCTSTR, LPCTSTR);
		PFUN_SetWindowTheme pFun = (PFUN_SetWindowTheme)GetProcAddress(hInst, "SetWindowTheme");
		if (pFun)
		{
			pFun(m_hWnd, _T(""), _T(""));//去掉XP主体
		}
		FreeLibrary(hInst);
	}
	hInst = LoadLibrary(_T("dwmapi.dll"));
	if (hInst)
	{
		typedef HRESULT (WINAPI *TmpFun)(HWND, DWORD, LPCVOID, DWORD);
		TmpFun DwmSetWindowAttributeEX = (TmpFun)GetProcAddress(hInst, "DwmSetWindowAttribute");
		if (DwmSetWindowAttributeEX)
		{
			DWORD dwAttr = 1;
			DwmSetWindowAttributeEX(GetSafeHwnd(), 2, &dwAttr, 4);//去掉Vista特效
		}
		FreeLibrary(hInst);
	}


	if (!m_imgBorder.IsNull()) return 0;
	if(m_captionPanel.m_hWnd) return 0;

	m_imgBorder.LoadFromResource(AfxGetInstanceHandle(), IDB_BORDER);
	m_captionPanel.Create(_T("CaptionPanel"), WS_CHILD|WS_VISIBLE, CRect(0, 0, lpCreateStruct->cx, 25), this, 10001);


	if(m_edit.m_hWnd) return 0;
	m_edit.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0, 1003, 0, 21), this, 10002);
	m_edit.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
	m_edit.SetWindowText("Finished loading");

	if(m_version_edit.m_hWnd) return 0;
	m_version_edit.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0, 1003, 0, 21), this, 10003);
	m_version_edit.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
	m_version_edit.SetWindowText("Hatchet @2015.03.03");

	pMaindlg =new CMainDlg;
	pMaindlg->Create(CMainDlg::IDD, this);
	pMaindlg->ShowWindow(SW_SHOW);

	return 0;
}

void CMainHatchet::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_captionPanel.m_hWnd == NULL) return;
	if(m_edit.m_hWnd == NULL) return;
	if(m_version_edit.m_hWnd == NULL) return;

	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcCaptionPanel = rcClient;
	rcCaptionPanel.bottom = 21;
	m_captionPanel.MoveWindow(rcCaptionPanel);

	CRect rcCEditPanel = rcClient;
	rcCEditPanel.top = rcClient.bottom-21;
	rcCEditPanel.right = rcCEditPanel.right - 150;
	m_edit.MoveWindow(rcCEditPanel);

	rcCEditPanel = rcClient;
	rcCEditPanel.top = rcClient.bottom-21;
	rcCEditPanel.left = rcCEditPanel.right - 150;
	m_version_edit.MoveWindow(rcCEditPanel);


	CRect rcMaindlg = rcClient;
	rcMaindlg.top = 21;
	rcMaindlg.bottom = rcMaindlg.bottom-20;
	pMaindlg->MoveWindow(rcMaindlg);

	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->MoveWindow(rcMaindlg);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CMainHatchet::AddFileTab()																				//文件管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}
	
	CFileDlg* pFileDlg = new CFileDlg();
	pFileDlg->Create(CFileDlg::IDD,this);
	m_PtrAllDlg.AddTail(pFileDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 1);
		pFileDlg->ShowWindow(SW_SHOW);
		pFileDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pFileDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddCmdTab()																				//CMD管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	CCmdDlg* pCmdDlg = new CCmdDlg();
	pCmdDlg->Create(CCmdDlg::IDD,this);
	m_PtrAllDlg.AddTail(pCmdDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 2);
		pCmdDlg->ShowWindow(SW_SHOW);
		pCmdDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pCmdDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddDataTab()																				//数据库管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	CDataDlg* pDataDlg = new CDataDlg();
	pDataDlg->Create(CDataDlg::IDD,this);
	m_PtrAllDlg.AddTail(pDataDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 3);
		pDataDlg->ShowWindow(SW_SHOW);
		pDataDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pDataDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddFileSaveTab()																			//文件内容管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	CFileSaveDlg* pFileSaveDlg = new CFileSaveDlg();
	pFileSaveDlg->Create(CFileSaveDlg::IDD,this);
	m_PtrAllDlg.AddTail(pFileSaveDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 4);
		pFileSaveDlg->ShowWindow(SW_SHOW);
		pFileSaveDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pFileSaveDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddProxyTab()																				//代理管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	CProxyDlg* pProxyDlg = new CProxyDlg();
	pProxyDlg->Create(CProxyDlg::IDD,this);
	m_PtrAllDlg.AddTail(pProxyDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 5);
		pProxyDlg->ShowWindow(SW_SHOW);
		pProxyDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pProxyDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddEvalCodeTab()																				//代码执行
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	CEvalCodeDlg* pEvalCodeDlg = new CEvalCodeDlg();
	pEvalCodeDlg->Create(CEvalCodeDlg::IDD,this);
	m_PtrAllDlg.AddTail(pEvalCodeDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 6);
		pEvalCodeDlg->ShowWindow(SW_SHOW);
		pEvalCodeDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pEvalCodeDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddRegistryTab()																				//注册表管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}

	CRegistryDlg* pRegistryDlg = new CRegistryDlg();
	pRegistryDlg->Create(CRegistryDlg::IDD,this);
	m_PtrAllDlg.AddTail(pRegistryDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 7);
		pRegistryDlg->ShowWindow(SW_SHOW);	
		pRegistryDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pRegistryDlg->MoveWindow(rcClient);
	}
}

void CMainHatchet::AddBrowseTab()																				//浏览器
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = (int)m_PtrAllDlg.GetCount()-1;

	pMaindlg->ShowWindow(SW_HIDE);
	for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
	{
		POSITION pos = m_PtrAllDlg.FindIndex(i);
		CDialog* pCDialog = NULL;
		if (pos)
		{
			pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			if (pCDialog != NULL)
			{
				pCDialog->ShowWindow(SW_HIDE);
			}
		}
	}


	CBrowseDlg* pBrowseDlg = new CBrowseDlg();
	pBrowseDlg->Create(CBrowseDlg::IDD,this);
	m_PtrAllDlg.AddTail(pBrowseDlg);
	if (bCloseTab)
	{
		bCloseTab = false;
		DeleteTab();
	}else{
		pCaptionCenterPanel->AddTab(ToShotUrl(szGlobal_SiteUrl), 8);
		pBrowseDlg->ShowWindow(SW_SHOW);
		pBrowseDlg->SetFocus();//切换标签，让焦点在里面

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = 21;
		rcClient.bottom = rcClient.bottom-20;
		pBrowseDlg->MoveWindow(rcClient);
	}
}

//////////////////////////////////////////////////////////////////////////

void CMainHatchet::SelchangeTab(int n)					//切换标签
{
	m_edit.SetWindowText( "" );					//设置状态栏

	nOldSelectPos = nNewSelectPos;		//记忆标签页数
	nNewSelectPos = n;

	if (n == -1)
	{
		pMaindlg->ShowWindow(SW_SHOW);
		pMaindlg->SetFocus();//切换标签，让焦点在里面

		for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
		{
			POSITION pos = m_PtrAllDlg.FindIndex(i);
			CDialog* pCDialog = NULL;
			if (pos)
			{
				pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
				if (pCDialog != NULL)
				{
					pCDialog->ShowWindow(SW_HIDE);
				}
			}
		}
	}else{
		pMaindlg->ShowWindow(SW_HIDE);

		for (int i=0; i<m_PtrAllDlg.GetCount(); i++)
		{
			POSITION pos = m_PtrAllDlg.FindIndex(i);
			CDialog* pCDialog = NULL;
			if (pos)
			{
				pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
				if (pCDialog != NULL)
				{
					pCDialog->ShowWindow(SW_HIDE);
					if (n == i)
					{
						pCDialog->ShowWindow(SW_SHOW);
						pCDialog->SetFocus();//切换标签，让焦点在里面
					}					
				}
			}
		}
	}
}

void CMainHatchet::DeleteTab(int n)				//删除标签
{
	POSITION pos;
	if (n == -1)		//-1 删除最后一个
	{
		pos = m_PtrAllDlg.FindIndex( m_PtrAllDlg.GetCount()-1 );
	}else{
		pos = m_PtrAllDlg.FindIndex(n);
	}
	
	if (pos)
	{
		CDialog* pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
		pCDialog->ShowWindow(SW_HIDE);
		delete pCDialog;
		pCDialog = NULL;
		m_PtrAllDlg.RemoveAt(pos);
	}else{
		//AfxMessageBox("CMainHatchet::DeleteTab ????");
		//return;
	}

	//////////////////////////////////////////////////////////////////////////	记忆显示标签页数

	pos = m_PtrAllDlg.FindIndex(nOldSelectPos);
	if (pos)
	{
		CDialog* pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
		nShowTab = nOldSelectPos;//现在显示的
		pCDialog->ShowWindow(SW_SHOW);
		pCDialog->SetFocus();//切换标签，让焦点在里面
	}else{
	//////////////////////////////////////////////////////////////////////////
		pos = m_PtrAllDlg.FindIndex(n-1);
		if (pos)
		{
			CDialog* pCDialog = (CDialog*)m_PtrAllDlg.GetAt(pos);
			nShowTab = n-1;//现在显示的
			pCDialog->ShowWindow(SW_SHOW);
			pCDialog->SetFocus();//切换标签，让焦点在里面
		}else{
			nShowTab = -1;//现在显示的
			pMaindlg->ShowWindow(SW_SHOW);
			pMaindlg->SetFocus();//切换标签，让焦点在里面
		}
	}
}

BOOL CMainHatchet::OnNcActivate(BOOL bActive)
{
	BOOL bRet = CWnd::OnNcActivate(bActive);

	Invalidate();
	SendMessage(WM_NCPAINT, 0, 0);
	return bRet;
}
