#pragma once
#include "afxwin.h"


// CProxyDlg 对话框

class CProxyDlg : public CDialog
{
	DECLARE_DYNAMIC(CProxyDlg)

public:
	CProxyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProxyDlg();

// 对话框数据
	enum { IDD = IDD_PROXY };

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void AddLog(CString strMsg);

protected:
	afx_msg void OnBnClickedButtonProxyStart();
	afx_msg void OnBnClickedButtonProxyStop();


protected:
	CString			szSiteUrl;							//Shell Url
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	int				nSiteScript;						//Shell 脚本类型

	CListBox m_proxy_log;
	HANDLE hThread;	
};
