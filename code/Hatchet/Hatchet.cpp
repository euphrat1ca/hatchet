// Hatchet.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Hatchet.h"
#include "MainHatchet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHatchetApp

BEGIN_MESSAGE_MAP(CHatchetApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHatchetApp construction

CHatchetApp::CHatchetApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHatchetApp object

CHatchetApp theApp;


// CHatchetApp initialization

BOOL CHatchetApp::InitInstance()
{
	CWinApp::InitInstance();
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////// 只运行一个
	HWND hHatchet = FindWindow("MAINCHatchet", "Hatchet");
	if (hHatchet  != NULL)
	{
		ShowWindow(hHatchet, SW_SHOWNORMAL);
		::SetForegroundWindow(hHatchet);	//把它设为前景窗口

		return CWinApp::ExitInstance();
	}
	//////////////////////////////////////////////////////////////////////////
	if(!AfxOleInit())
	{
		AfxMessageBox("OLE Error!");
		return FALSE;
	}
	AfxEnableControlContainer();						//注意，添加ActiveX，不显示对话框。

	CMainHatchet* pMainHatchet = new CMainHatchet();
	pMainHatchet->CreateEx(0, _T("MAINCHatchet"), _T("Hatchet"),
		WS_POPUPWINDOW | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL);
	m_pMainWnd = pMainHatchet;
	pMainHatchet->CenterWindow();
	pMainHatchet->ShowWindow(SW_SHOW);
	pMainHatchet->UpdateWindow();

	return TRUE;
}

int CHatchetApp::ExitInstance()
{
	if (m_pMainWnd)
	{
		delete ((CMainHatchet*)m_pMainWnd);
	}
	return CWinApp::ExitInstance();
}
