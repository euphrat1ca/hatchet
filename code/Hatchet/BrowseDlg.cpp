// BrowseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "BrowseDlg.h"


// CBrowseDlg 对话框

IMPLEMENT_DYNAMIC(CBrowseDlg, CDialog)

CBrowseDlg::CBrowseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrowseDlg::IDD, pParent)
{

}

CBrowseDlg::~CBrowseDlg()
{
	DestroyWindow();
}

void CBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_explorer);
	DDX_Control(pDX, IDC_COMBO_ADDRESS, m_com_browse);
}


BEGIN_MESSAGE_MAP(CBrowseDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BROWSE_OK, &CBrowseDlg::OnBnClickedBrowseOk)
END_MESSAGE_MAP()


// CBrowseDlg 消息处理程序

BOOL CBrowseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_COMBO_ADDRESS)->SetFont(&m_FontSample);

	SetDlgItemText(IDC_COMBO_ADDRESS, szGlobal_SiteUrl);
	m_edit.SetWindowText( "Browse" );					//设置状态栏

	m_com_browse.AddString(szGlobal_SiteUrl);
	RECT mRect;
	m_com_browse.GetWindowRect(&mRect);
	m_com_browse.SetWindowPos(NULL,0, 0, mRect.right-mRect.left, 200, SWP_NOMOVE|SWP_NOREDRAW);

	m_explorer.Navigate(szGlobal_SiteUrl, NULL, NULL, NULL, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CBrowseDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		if(pMsg -> wParam == VK_RETURN)
		{
			if ((GetFocus()->GetParent()) == GetDlgItem(IDC_COMBO_ADDRESS))
			{
				OnBnClickedBrowseOk();
			}
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CBrowseDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	CRect	rcCWnd = rcClient;
	CWnd*	pCWnd   =   GetDlgItem(IDC_COMBO_ADDRESS);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.left = 2;
		rcCWnd.bottom = 23;
		rcCWnd.right = rcCWnd.right - 77;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_BROWSE_OK);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.left = rcCWnd.right-75;
		rcCWnd.bottom = 23;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_EXPLORER1);
	if (pCWnd != NULL)
	{
		rcCWnd.top = rcCWnd.top + 25;
		pCWnd->MoveWindow(rcCWnd);
	}
}

void CBrowseDlg::OnBnClickedBrowseOk()						//地址跳转
{
	CString temp;
	GetDlgItemText(IDC_COMBO_ADDRESS, temp);
	m_com_browse.AddString(temp);
	m_explorer.Navigate(temp, NULL, NULL, NULL, NULL);
}
//////////////////////////////////////////////////////////////////////////
BEGIN_EVENTSINK_MAP(CBrowseDlg, CDialog)
	ON_EVENT(CBrowseDlg, IDC_EXPLORER1, 102, CBrowseDlg::StatusTextChangeExplorer1, VTS_BSTR)
	ON_EVENT(CBrowseDlg, IDC_EXPLORER1, 259, CBrowseDlg::DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()


void CBrowseDlg::StatusTextChangeExplorer1(LPCTSTR Text)
{
	m_edit.SetWindowText( Text );					//设置状态栏
}


void CBrowseDlg::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	SetDlgItemText( IDC_COMBO_ADDRESS, COLE2T(URL->bstrVal) );
}
