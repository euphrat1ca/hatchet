// SetpSiteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "SetpSiteDlg.h"


// CSetpSiteDlg 对话框

IMPLEMENT_DYNAMIC(CSetpSiteDlg, CDialog)

CSetpSiteDlg::CSetpSiteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetpSiteDlg::IDD, pParent)
{

}

CSetpSiteDlg::~CSetpSiteDlg()
{
}

void CSetpSiteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NCODEPAGE, m_com_ncodepage);
	DDX_Control(pDX, IDC_COMBO_NSCRIPT, m_com_nscript);
	DDX_Control(pDX, IDC_COMBO_NTYPE, m_com_ntype);
	DDX_Control(pDX, IDC_COMBO_JOIN, m_com_join);
}


BEGIN_MESSAGE_MAP(CSetpSiteDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetpSiteDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_JOIN, &CSetpSiteDlg::OnBnClickedCheckJoin)
END_MESSAGE_MAP()


// CSetpSiteDlg 消息处理程序

BOOL CSetpSiteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_SETPSITE_URL)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_SETPSITE_PASS)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_SETPSITE_CONFIG)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_NTYPE)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_NSCRIPT)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_NCODEPAGE)->SetFont(&m_FontSample);

	GetDlgItem(IDC_COMBO_JOIN)->SetFont(&m_FontSample);
	GetDlgItem(IDC_CHECK_JOIN)->SetFont(&m_FontSample);

	GetDlgItem(IDOK)->SetFont(&m_FontSample);


	RECT rect;

	MydbArray.RemoveAll();
	pMydb->Select("SELECT [TypeName] FROM [Type]", MydbArray);
	int i=0;
	while( i < MydbArray.GetSize())	// 有没有到表结尾
	{
		m_com_ntype.AddString( MydbArray.GetAt(i) );
		i++;
	}
	m_com_ntype.GetWindowRect(&rect);
	m_com_ntype.SetWindowPos(NULL,0,0,rect.right - rect.left,100,SWP_NOMOVE|SWP_NOREDRAW);

	m_com_nscript.AddString("ASP");		//1
	m_com_nscript.AddString("ASPX");	//2
	m_com_nscript.AddString("PHP");		//3
	m_com_nscript.AddString("JSP");		//4
	
	m_com_nscript.GetWindowRect(&rect);
	m_com_nscript.SetWindowPos(NULL,0,0,rect.right - rect.left,100,SWP_NOMOVE|SWP_NOREDRAW);


	m_com_ncodepage.AddString("UTF-8");
	m_com_ncodepage.AddString("GB2312");
	m_com_ncodepage.AddString("BIG5");
	m_com_ncodepage.SetWindowPos(NULL,0,0,rect.right - rect.left,100,SWP_NOMOVE|SWP_NOREDRAW);


	pMydb->Select("SELECT [SiteUrl] FROM [SiteTable]", MydbArray);
	i=0;
	while( i < MydbArray.GetSize())	// 有没有到表结尾
	{
		m_com_join.AddString( MydbArray.GetAt(i) );

		i++;
	}
	m_com_join.GetWindowRect(&rect);
	m_com_join.SetWindowPos(NULL,0,0,rect.right - rect.left,100,SWP_NOMOVE|SWP_NOREDRAW);

	


	SetDlgItemText(IDC_EDIT_SETPSITE_URL,"http://");

	if (nSiteID > 0)					//编辑
	{
		SetWindowText("Edit WebSite");
		temp.Format("%d", nSiteID);
		temp = "Select nType,nCodePage,SiteUrl,SitePass,Config,nScript,JoinUrl from [SiteTable] where ID="+ temp;
		pMydb->Select(temp, MydbArray);
		temp = MydbArray.GetAt(1);
		if (temp == "65001")
		{
			m_com_ncodepage.SetCurSel(0);
		}else if (temp == "936")
		{
			m_com_ncodepage.SetCurSel(1);
		}else if (temp == "950")
		{
			m_com_ncodepage.SetCurSel(2);
		}else{
			m_com_ncodepage.SetCurSel(3);
		}

		SetDlgItemText(IDC_EDIT_SETPSITE_URL, MydbArray.GetAt(2));
		SetDlgItemText(IDC_EDIT_SETPSITE_PASS, MydbArray.GetAt(3));
		SetDlgItemText(IDC_EDIT_SETPSITE_CONFIG, MydbArray.GetAt(4));

		temp = MydbArray.GetAt(5);
		if (temp == "1")
		{
			m_com_nscript.SetCurSel(0);
		}else if (temp == "2")
		{
			m_com_nscript.SetCurSel(1);
		}else if (temp == "3")
		{
			m_com_nscript.SetCurSel(2);
		}else if (temp == "4")
		{
			m_com_nscript.SetCurSel(3);
		}
		//////////////////////////////////////////////////////////////////////////	设置join
		CString szFindJoinUrl = MydbArray.GetAt(6);
		if (szFindJoinUrl != "")
		{
			GetDlgItem(IDC_COMBO_JOIN)->EnableWindow(TRUE);
			((CButton*)(GetDlgItem( IDC_CHECK_JOIN )))->SetCheck( TRUE );
			m_com_join.SetCurSel( m_com_join.FindStringExact( 0, szFindJoinUrl ) );	//查找全部 FindStringExact != FindString
		}	
		//////////////////////////////////////////////////////////////////////////	设置类型
		pMydb->Select("SELECT [TypeName] FROM [Type] Where [ID]="+ MydbArray.GetAt(0), MydbArray);
		for (int i=0; i < m_com_ntype.GetCount(); i++)
		{
			m_com_ntype.GetLBText(i, temp);
			if (temp == MydbArray.GetAt(0))
			{
				m_com_ntype.SetCurSel(i);
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}else{									//添加
		SetWindowText("Add WebSite");
		if (szSelect_Type == "")
		{
			m_com_ntype.SetCurSel(0);
		}else{
			for (int i=0; i < m_com_ntype.GetCount(); i++)
			{
				m_com_ntype.GetLBText(i, temp);
				if (temp == szSelect_Type)
				{
					m_com_ntype.SetCurSel(i);
					break;
				}
			}
		}
		m_com_ncodepage.SetCurSel(0);
		m_com_nscript.SetCurSel(2);
	}
	
	GetDlgItem(IDC_EDIT_SETPSITE_URL)->SetFocus();
	((CEdit *)GetDlgItem(IDC_EDIT_SETPSITE_URL))->SetSel(0, -1);	//加载完成窗体,设置一下光标

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CSetpSiteDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && ::GetKeyState(VK_CONTROL)&0x8000)
		{
			if ( GetDlgItem(IDC_EDIT_SETPSITE_URL) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_SETPSITE_URL))->SetSel( 0, -1);			//全选
			}
			if ( GetDlgItem(IDC_EDIT_SETPSITE_PASS) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_SETPSITE_PASS))->SetSel( 0, -1);			//全选
			}
			if ( GetDlgItem(IDC_EDIT_SETPSITE_CONFIG) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_EDIT_SETPSITE_CONFIG))->SetSel( 0, -1);			//全选
			}
		}
	}
	if (pMsg->message == WM_KEYUP)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT_SETPSITE_URL))
		{
			CString szUrl,szUrlLower;
			GetDlgItemText(IDC_EDIT_SETPSITE_URL, szUrl);
			szUrlLower = szUrl;
			szUrlLower.MakeLower();
			int posX = rfind(szUrlLower, ".");
			if (posX > 2)
			{
				szUrlLower = szUrlLower.Right(posX-1);
				if (szUrlLower == "aspx")
				{
					m_com_nscript.SetCurSel(1);
				}else if (szUrlLower == "asp")
				{
					m_com_nscript.SetCurSel(0);
				}else if (szUrlLower == "php")
				{
					m_com_nscript.SetCurSel(2);
				}else if (szUrlLower == "jsp")
				{
					m_com_nscript.SetCurSel(3);
				}
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CSetpSiteDlg::OnOK() 
{
	CString szOnOk = szSelect_Type;
	szOnOk.Replace("'", "''");
	if (nSiteID > 0)					//编辑
	{
		if ( OnSiteEdit() )
		{
			pCMainDlg->ShowSiteList(szOnOk);
			CDialog::OnOK();
		}
	}else{									//添加
		if ( OnSiteAdd() )
		{
			pCMainDlg->ShowSiteList(szOnOk);
			CDialog::OnOK();
		}
	}
}


void CSetpSiteDlg::PostNcDestroy()
{
	delete this;
	CDialog::PostNcDestroy();
}


BOOL CSetpSiteDlg::OnSiteAdd()								//添加Shell
{
	CString szAddCodePage = "";
	GetDlgItemText(IDC_COMBO_NCODEPAGE, temp);
	if (temp == "UTF-8")
	{
		szAddCodePage = "65001";
	}else if (temp == "GB2312")
	{
		szAddCodePage = "936";
	}else if (temp == "BIG5")
	{
		szAddCodePage = "950";
	}

	CString szAddSiteUrl;
	GetDlgItemText(IDC_EDIT_SETPSITE_URL, szAddSiteUrl);
	szAddSiteUrl.Replace("'", "''");

	CString szAddJoinUrl="";
	if ( GetDlgItem(IDC_COMBO_JOIN)->IsWindowEnabled() )//按钮可用
	{
		GetDlgItemText(IDC_COMBO_JOIN, szAddJoinUrl);
		szAddJoinUrl.Replace("'", "''");
	}

	CString szAddSitePass="";
	GetDlgItemText(IDC_EDIT_SETPSITE_PASS, szAddSitePass);
	szAddSitePass.Replace("'", "''");

	CString szAddConfig="";
	GetDlgItemText(IDC_EDIT_SETPSITE_CONFIG, szAddConfig);
	szAddConfig.Replace("'", "''");

	CString szAddScript = "";
	GetDlgItemText(IDC_COMBO_NSCRIPT, temp);
	if (temp == "ASP")
	{
		szAddScript = "1";
	}else if (temp == "ASPX")
	{
		szAddScript = "2";
	}else if (temp == "PHP")
	{
		szAddScript = "3";
	}else if (temp == "JSP")
	{
		szAddScript = "4";
	}

	if (szAddCodePage == "" || szAddSiteUrl == "" || szAddSitePass == "" || szAddScript == "")
	{
		AfxMessageBox("Add Site Error!");
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////	判断是否存在数据库里
	pMydb->Select("SELECT [SitePass] from [SiteTable] where [SiteUrl]='"+ szAddSiteUrl +"'", MydbArray);
	if (MydbArray.GetSize() != 0)		// 没有缓存数据
	{
		AfxMessageBox("URL Already exists!");
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	GetDlgItemText(IDC_COMBO_NTYPE, temp);
	MydbArray.RemoveAll();
	temp.Replace("'", "''");
	pMydb->Select("SELECT [ID] FROM [Type] Where [TypeName]='"+ temp +"'", MydbArray);
	//////////////////////////////////////////////////////////////////////////
	return (pMydb->Execute("INSERT INTO [SiteTable] ([nType],[nCodePage],[SiteUrl],[JoinUrl],[SitePass],[Config],[IP],[nScript],[NewTime],[AddTime]) VALUES ("
		+ MydbArray.GetAt(0) +","+szAddCodePage+",'"+szAddSiteUrl+"','"+szAddJoinUrl+"','"+szAddSitePass+"','"+szAddConfig+"','','"+szAddScript+"',NOW(),NOW())"));
}


BOOL CSetpSiteDlg::OnSiteEdit()											//编辑Shell
{
	CString szEditCodePage = "";
	GetDlgItemText(IDC_COMBO_NCODEPAGE, temp);
	if (temp == "UTF-8")
	{
		szEditCodePage = "65001";
	}else if (temp == "GB2312")
	{
		szEditCodePage = "936";
	}else if (temp == "BIG5")
	{
		szEditCodePage = "950";
	}

	CString szEditSiteUrl;
	GetDlgItemText(IDC_EDIT_SETPSITE_URL, szEditSiteUrl);
	szEditSiteUrl.Replace("'", "''");

	CString szEditJoinUrl="";
	if ( GetDlgItem(IDC_COMBO_JOIN)->IsWindowEnabled() )//按钮可用
	{
		GetDlgItemText(IDC_COMBO_JOIN, szEditJoinUrl);
		szEditJoinUrl.Replace("'", "''");
	}

	CString szEditSitePass;
	GetDlgItemText(IDC_EDIT_SETPSITE_PASS, szEditSitePass);
	szEditSitePass.Replace("'", "''");

	CString szEditConfig="";
	GetDlgItemText(IDC_EDIT_SETPSITE_CONFIG, szEditConfig);
	szEditConfig.Replace("'", "''");

	CString szEditScript = "";
	GetDlgItemText(IDC_COMBO_NSCRIPT, temp);
	if (temp == "ASP")
	{
		szEditScript = "1";
	}else if (temp == "ASPX")
	{
		szEditScript = "2";
	}else if (temp == "PHP")
	{
		szEditScript = "3";
	}else if (temp == "JSP")
	{
		szEditScript = "4";
	}
	temp.Format("%d", nSiteID);

	//////////////////////////////////////////////////////////////////////////
	CString temp2;
	GetDlgItemText(IDC_COMBO_NTYPE, temp2);
	temp2.Replace("'", "''");
	MydbArray.RemoveAll();
	pMydb->Select("SELECT [ID] FROM [Type] Where [TypeName]='"+ temp2 +"'", MydbArray);
	//////////////////////////////////////////////////////////////////////////

	return (pMydb->Execute("UPDATE [SiteTable] SET [nCodePage] ="+ szEditCodePage +
		",[SiteUrl] = '"+ szEditSiteUrl +
		"',[JoinUrl] = '"+ szEditJoinUrl +
		"',[SitePass] = '"+ szEditSitePass +
		"',[Config] = '"+ szEditConfig +
		"',[nType] = '"+ MydbArray.GetAt(0) +
		"',[nScript] = " + szEditScript +" where [id]="+ temp));
}


void CSetpSiteDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CSetpSiteDlg::OnBnClickedCheckJoin()
{
	if ( GetDlgItem(IDC_COMBO_JOIN)->IsWindowEnabled() )//按钮可用
	{
		GetDlgItem(IDC_COMBO_JOIN)->EnableWindow(FALSE);
	}else{
		GetDlgItem(IDC_COMBO_JOIN)->EnableWindow(TRUE);
	}
}
