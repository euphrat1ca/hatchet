#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CRegistryDlg 对话框

class CRegistryDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegistryDlg)

public:
	CRegistryDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegistryDlg();

// 对话框数据
	enum { IDD = IDD_REGISTRY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl m_registry_tree;
	CListCtrl m_registry_list;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedRegistryButton();
	afx_msg void OnTvnSelchangingRegistryTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRegistryGet(CString szPath, bool bNoSelectTempDB = false);
	afx_msg void OnBnClickedRegistryRadioCmd();
	afx_msg void OnBnClickedRegistryRadioWscript();
	afx_msg void OnNMRclickRegistryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRegistryRefresh();				//刷新
	afx_msg void OnRegistryClearCache();			//清空缓存

public:
	CString			szSiteUrl;							//Shell Url
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	int				nSiteScript;						//Shell 脚本类型
	CComboBox		m_registry_com;
	bool			bThreadFinish;						//判断是否上条命令执行完毕
	CString			RegistryGetData_szPath;
	bool			RegistryGetData_bNoDB;				//不要进缓存	
};
