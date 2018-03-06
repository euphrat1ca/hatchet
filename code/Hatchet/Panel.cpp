// Panel.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "Panel.h"


// CPanel

IMPLEMENT_DYNAMIC(CPanel, CWnd)

CPanel::CPanel()
{

}

CPanel::~CPanel()
{
}


BEGIN_MESSAGE_MAP(CPanel, CWnd)
END_MESSAGE_MAP()



// CPanel 消息处理程序



BOOL CPanel::PreCreateWindow(CREATESTRUCT& cs)
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
	//wcex.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wcex.hbrBackground = CreateSolidBrush(RGB(51, 102, 102));		//标题背景颜色
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = cs.lpszClass;
	bRet = RegisterClassEx(&wcex);
	return bRet;
}

BOOL CPanel::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(_T("CPanel"), lpszWindowName, dwStyle, rect, pParentWnd, nID);
}