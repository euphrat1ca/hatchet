#pragma once
#include "afxwin.h"


// CFileDlg 对话框

class CFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileDlg();

// 对话框数据
	enum { IDD = IDD_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg bool GetServerInfo();
	afx_msg void OnFileClearCache();			//清空缓存
	afx_msg void OnFileNewDirectory();			//新建目录
	afx_msg void OnFileNewFile();				//新建文件
	afx_msg void OnFileRename();				//重命名
	afx_msg void OnFileChangeTime();			//更改时间
	afx_msg void OnFileEditFile();				//编辑文件
	afx_msg void OnFileDeleteFiles();			//删除文件
	afx_msg void OnFileWget();					//WGET
	afx_msg void OnFileUploadFile();			//上传文件
	afx_msg void OnFileDownloadFile();			//下载文件

	afx_msg void OnFileCut();					//文件剪切
	afx_msg void OnFileCopy();					//文件复制
	afx_msg void OnFilePaste();					//文件粘贴

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedReadfiles();
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocusSubItemEdit();
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//afx_msg void OnTvnSelchangedFileTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingFileTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnFileRefresh();				//刷新

	CListCtrl		m_filelist;
	CTreeCtrl		m_dirtree;
	CEdit			m_SubItemEdit;
	CComboBox		m_com_file_path;


	CImageList		m_iImageTree;				//图标链
	CImageList		m_iImageList;				//图标链
	CStringArray	szFilesArrCont;				//文件类型图标存在 FilesIco 的第几个
	HTREEITEM		m_hNowItem;					//减少性能，不然每次都要循环查找
	bool			IsWin;						//是否WIN系统 (/) 换成 (\)
	CString			WWWRoot;					//缓存技术，暂时还没有，有了，需要修改

	CString			szSystemInfo;						//Windows NT MY-785137BA5775 5.2 build 3790 (Windows Server 2003 Enterprise Edition Service Pack 2) i586(SYSTEM)
														//Linux mail.baidu.com 2.6.18-194.el5 #1 SMP Fri Apr 2 14:58:35 EDT 2010 i686(apache)
	CString			szSiteUrl;							//Shell Url
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	CString			szSiteCode;							//JSP 编码
	CString			szSiteConfig;						//Shell Config
	int				nSiteScript;						//Shell 脚本类型

	CString			szFileFamatPageCode;				//格式化TEMP
	CString			szCacheItem;						//缓存技术

	int				nSelectList;						//就地编辑需要用到

	CString			szFileCutName;						//文件剪切 对方为空判断
	CString			szFileCutPath;						//文件剪切 对方为空判断
	CString			szFileCopyName;						//文件复制 对方为空判断
	CString			szFileCopyPath;						//文件复制 对方为空判断

	bool			bThreadFinish;						//判断是否上条命令执行完毕

	CString			GetFiles_szPath;
	bool			GetFiles_bNoDB;// = false
public:
	afx_msg void OnBnClickedUpperstory();
};
