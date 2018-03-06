#pragma once
#include "afxwin.h"


// CDataDlg 对话框

class CDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataDlg)

public:
	CDataDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataDlg();

// 对话框数据
	enum { IDD = IDD_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonExecsql();
	afx_msg void OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocusSubItemEdit();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDataGetcount();			//获取表的个数
	afx_msg void OnDataGet20Lines();		//获取20条数据
	afx_msg void OnDataDropTable();			//删除表
	afx_msg void OnDataSetclipboard();
	afx_msg void OnDataExportHtml();		//导出HTML
	afx_msg void OnDataExportText();		//导出TEXT
	/*
	afx_msg void GetDataBases();
	afx_msg void GetTables(HTREEITEM hParent, CString szDataName);
	afx_msg void GetColumns(HTREEITEM hParent, CString szDataName, CString szTableName);
	afx_msg void GetExecSql(CString szSql);
	*/
	afx_msg void OnBnClickedDataConfig();

public:
	CString			sz_datahost,sz_datauser,sz_datapass;
	CImageList		m_iImageTree;						//图标链
	HTREEITEM		select_ht;							//在焦点的Tree 添加 编辑 删除 需要用到
	CStringArray	CtrlListHeaderArr;


	CString			szSystemInfo;						//Windows NT MY-785137BA5775 5.2 build 3790 (Windows Server 2003 Enterprise Edition Service Pack 2) i586(SYSTEM)
														//Linux mail.baidu.com 2.6.18-194.el5 #1 SMP Fri Apr 2 14:58:35 EDT 2010 i686(apache)
	CString			szSiteUrl;							//Shell Url
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	CString			szSiteCode;							//JSP	编码
	CString			szSiteConfig;						//Shell Config
	int				nSiteScript;						//Shell 脚本类型

	CTreeCtrl		m_datatree;
	CListCtrl		m_datalist;
	CComboBox		m_comDatas;
	CEdit			m_SubItemEdit;
	CComboBox		m_com_sql;

	bool			bThreadFinish;						//判断是否上条命令执行完毕

	HTREEITEM		GetTables_hParent;
	CString			GetTables_szDataName;

	HTREEITEM		GetColumns_hParent;
	CString			GetColumns_szDataName;
	CString			GetColumns_szTableName;

	CString			GetExecSql_szSql;
};
