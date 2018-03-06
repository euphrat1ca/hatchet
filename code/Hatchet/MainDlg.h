#pragma once


// CMainDlg 对话框

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
	enum { IDD = IDD_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMRClickSitelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTypetree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkSitelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTypetree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTypetree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTypetree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnColumnclickSitelist(NMHDR *pNMHDR, LRESULT *pResult);//排序

public:
	bool ShowSiteList(CString szTypeName=""/*分类名称*/, CString szFind=""/*查找关键词*/, CString szSort="NewTime"/*哪个列*/, CString szDesc_Asc="Desc");
	afx_msg void OnMainSiteAdd();			//添加
	afx_msg void OnMain2SiteAdd();			//添加

	afx_msg void OnMainSiteEdit();			//编辑
	afx_msg void OnMainSiteDelete();		//删除
	afx_msg void OnMainCopyUrl();			//复制站点
	afx_msg void OnMainSiteFind();			//查找
	afx_msg void OnMainSite2Find();			//查找	

	afx_msg void OnMainUpdateIni(bool bNoCreat=true);			//更新INI
	afx_msg void OnMainSiteUpdateIni();		//更新INI
	afx_msg void OnMainSite2UpdateIni();	//更新INI
	afx_msg void OnMainUpdatePost();		//更新所有POST代码

	afx_msg void OnHatchetTypeAdd();
	afx_msg void OnHatchetTypeEdit();
	afx_msg void OnHatchetTypeDel();

	afx_msg void AddFileTab();
	afx_msg void AddCmdTab();
	afx_msg void AddDataTab();
	afx_msg void AddProxyTab();
	afx_msg void AddEvalCodeTab();
	afx_msg void AddRegistryTab();
	afx_msg void AddBrowseTab();

protected:
	HTREEITEM	select_ht;				//在焦点的Tree 添加 编辑 删除 需要用到
	int			nHatchetTypeAdd_Edit;	//失去焦点的时候，判断到底要不要 1=add 2=edit
	CString		szSelect_Type;			//添加shell的时候，默认显示当前类型

	CString		szSort_TypeName/*分类名称*/,szSort_Find/*查找关键词*/,szSort_Sort/*哪个列*/,szSort_Desc_Asc;//排序，记忆。
public:
	CListCtrl	m_sitelist;
	CEdit		m_Edit_Find;
	CTreeCtrl	m_typetree;
	CImageList	m_iImageList;						//图标链
	CImageList	m_iImageList_Type;					//图标链

};
