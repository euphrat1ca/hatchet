#pragma once

#include "MainDlg.h"
#include "afxwin.h"
// CSetpSiteDlg 对话框

class CSetpSiteDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetpSiteDlg)

public:
	CSetpSiteDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetpSiteDlg();
	int nSiteID;
	CString	szSelect_Type;//添加shell的时候，默认显示当前类型
	CMainDlg *pCMainDlg;

// 对话框数据
	enum { IDD = IDD_SETPSITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
	
protected:
	CComboBox	m_com_ncodepage;
	CComboBox	m_com_nscript;
	CComboBox	m_com_ntype;
	CComboBox	m_com_join;

	afx_msg BOOL OnSiteAdd();
	afx_msg BOOL OnSiteEdit();
	virtual void OnOK();
	CString temp;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckJoin();
};
