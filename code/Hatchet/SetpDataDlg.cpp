// SetpDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "SetpDataDlg.h"


// CSetpDataDlg 对话框

IMPLEMENT_DYNAMIC(CSetpDataDlg, CDialog)

CSetpDataDlg::CSetpDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetpDataDlg::IDD, pParent)
{

}

CSetpDataDlg::~CSetpDataDlg()
{
}

void CSetpDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEMO, m_com_demo);
}


BEGIN_MESSAGE_MAP(CSetpDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetpDataDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_DEMO, &CSetpDataDlg::OnCbnSelchangeComboDemo)
END_MESSAGE_MAP()


// CSetpDataDlg 消息处理程序

BOOL CSetpDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_DEMO)->SetFont(&m_FontSample);
	GetDlgItem(IDC_DATA_SETP)->SetFont(&m_FontSample);

	// TODO:  在此添加额外的初始化
	szSiteUrl.Replace("'", "''");
	pMydb->Select("SELECT Config FROM SiteTable Where SiteUrl='"+szSiteUrl+"'", MydbArray);
	if(MydbArray.GetCount() > 0)
	{
		SetDlgItemText(IDC_DATA_SETP, MydbArray.GetAt(0));
	}

	if (nSiteScript == 3)//PHP
	{
		m_com_demo.AddString("<T>MYSQL</T>\r\n<H>localhost</H>\r\n<U>root</U>\r\n<P></P>\r\n<L>utf8</L>");
		m_com_demo.AddString("<T>MSSQL</T>\r\n<H>localhost</H>\r\n<U>sa</U>\r\n<P></P>");
		m_com_demo.AddString("<T>ORACLE</T>\r\n<H>localhost</H>\r\n<U>dbuser</U>\r\n<P>dbpwd</P>");
		m_com_demo.AddString("<T>PostgreSQL</T>\r\n<H>localhost</H>\r\n<S>5432</S>\r\n<D>dbname</D>\r\n<U>user</U>\r\n<P>pwd</P>");
		m_com_demo.AddString("<T>INFORMIX</T>\r\n<H>localhost</H>\r\n<U>dbuser</U>\r\n<P>dbpwd</P>");
		m_com_demo.AddString("<T>ODBC_MSSQL</T>\r\n<H>dsn</H>\r\n<U>user</U>\r\n<P>password</P>");
		m_com_demo.AddString("<T>PDO_MYSQL</T>\r\n<H>localhost</H>\r\n<U>root</U>\r\n<P></P>");
		m_com_demo.AddString("<T>PDO_MSSQL</T>\r\n<H>localhost</H>\r\n<U>sa</U>\r\n<P></P>");
	}else if (nSiteScript == 4)//JSP
	{
		/*
		m_com_demo.AddString("<T>XDB</T>\r\n<X>\r\ncom.microsoft.sqlserver.jdbc.SQLServerDriver\r\njdbc:sqlserver://127.0.0.1:1433;databaseName=test;user=sa;password=123456\r\n</X>");
		m_com_demo.AddString("<T>XDB</T>\r\n<X>\r\ncom.mysql.jdbc.Driver\r\njdbc:mysql://localhost/test?user=root&password=123456\r\n</X>");
		m_com_demo.AddString("<T>XDB</T>\r\n<X>\r\noracle.jdbc.driver.OracleDriver\r\njdbc:oracle:thin:user/password@127.0.0.1:1521/test\r\n</X>");
		m_com_demo.AddString("<T>XDB</T>\r\n<X>Data Source=.;Initial Catalog=master;Persist Security Info=True;User Id=sa;Pwd=\r\n</X>");
		*/
		m_com_demo.AddString("<T>ORACLE</T>\r\n<X>\r\noracle.jdbc.driver.OracleDriver\r\njdbc:oracle:thin:user/password@//127.0.0.1:1521/test\r\n</X>");
		m_com_demo.AddString("<T>MYSQL</T>\r\n<X>\r\ncom.mysql.jdbc.Driver\r\njdbc:mysql://127.0.0.1/test?user=root&password=root\r\n</X>");
		m_com_demo.AddString("<T>MSSQL2000</T>\r\n<X>\r\ncom.microsoft.jdbc.sqlserver.SQLServerDriver\r\njdbc:sqlserver://127.0.0.1:1433;databaseName=master;user=sa;password=123456\r\n</X>");
		m_com_demo.AddString("<T>MSSQL2005</T>\r\n<X>\r\ncom.microsoft.sqlserver.jdbc.SQLServerDriver\r\njdbc:sqlserver://127.0.0.1:1433;databaseName=master;user=sa;password=123456\r\n</X>");
		m_com_demo.AddString("<T>Db2</T>\r\n<X>\r\ncom.ibm.db2.jcc.DB2Driver\r\njdbc:db2://127.0.0.1:50000/mydb:user=dbadm;password=123456;\r\n</X>");
		m_com_demo.AddString("<T>Informix</T>\r\n<X>\r\ncom.informix.jdbc.IfxDriver\r\njdbc:informix-sqli://127.0.0.1:1234/dbname:INFORMIXSERVER=servername;user=username;password=123456\r\n</X>");
		m_com_demo.AddString("<T>Sybase</T>\r\n<X>\r\ncom.sybase.jdbc.SybDriver\r\njdbc:sybase:Tds:127.0.0.1:1234/dbname?user=username&password=123456\r\n</X>");
		m_com_demo.AddString("<T>PostgreSQL</T>\r\n<X>\r\norg.postgresql.Driver\r\njdbc:postgresql://127.0.0.1:5432/dbname?user=username&password=123456\r\n</X>");
		m_com_demo.AddString("<T>Access</T>\r\n<X>\r\njdbc:odbc:DRIVER={Microsoft Access Driver (*.mdb)};DBQ=data.mdb\r\n</X>");
		m_com_demo.AddString("<T>Excel</T>\r\n<X>\r\njdbc:odbc:Driver={Microsoft Excel Driver (*.xls)};DBQ=data.xls\r\n</X>");
	}else{
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\\test.mdb</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\\test.mdb</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>");
		m_com_demo.AddString("<T>ADO</T>\r\n<C>Dsn=DsnName;</C>");
	}

	RECT rect;
	m_com_demo.GetWindowRect(&rect);
	m_com_demo.SetWindowPos(NULL, 0, 0, rect.right - rect.left, 200, SWP_NOMOVE | SWP_NOREDRAW);

	GetDlgItem(IDC_DATA_SETP)->SetFocus();
	((CEdit *)GetDlgItem(IDC_DATA_SETP))->SetSel(-1);	//加载完成窗体,设置一下光标到最后

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetpDataDlg::OnBnClickedOk()
{
	CString szDataConfig;
	GetDlgItemText(IDC_DATA_SETP, szDataConfig);
	szDataConfig.Replace("'", "''");
	if ( pMydb->Execute("UPDATE [SiteTable] SET [Config]='"+szDataConfig+"' Where SiteUrl='"+szSiteUrl+"'") )
	{
		szGlobal_SiteConfig = szDataConfig;
		OnOK();
	}else{
		AfxMessageBox("Fail!");
	}	
}


void CSetpDataDlg::OnCbnSelchangeComboDemo()
{
	CString	temp, temp2;	//选择的数据库
	m_com_demo.GetLBText(m_com_demo.GetCurSel(), temp);		//获取现在选择的数据库

	GetDlgItemText(IDC_DATA_SETP, temp2);

	SetDlgItemText(IDC_DATA_SETP, temp +"\r\n\r\n"+ temp2);
}

BOOL CSetpDataDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && ::GetKeyState(VK_CONTROL)&0x8000)
		{
			if ( GetDlgItem(IDC_DATA_SETP) == GetFocus() )						//全选
			{
				((CEdit *)GetDlgItem(IDC_DATA_SETP))->SetSel( 0, -1);			//全选
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}