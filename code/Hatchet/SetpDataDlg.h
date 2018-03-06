#pragma once
#include "afxwin.h"


// CSetpDataDlg 对话框

class CSetpDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetpDataDlg)

public:
	CSetpDataDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetpDataDlg();

// 对话框数据
	enum { IDD = IDD_SETPDATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDemo();
	DECLARE_MESSAGE_MAP()

	CComboBox m_com_demo;

public:
	CString			szSiteUrl;							//Shell Url
	int				nSiteScript;						//Shell 脚本类型
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
