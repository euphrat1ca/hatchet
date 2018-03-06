#pragma once


// CWgetDlg 对话框

class CWgetDlg : public CDialog
{
	DECLARE_DYNAMIC(CWgetDlg)

public:
	CWgetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWgetDlg();

// 对话框数据
	enum { IDD = IDD_WGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	CString szWgetPath;
	afx_msg void OnBnClickedOnWget();
	afx_msg void OnEnSetfocusEditWgetUrl();


	CString			szSiteUrl;							//Shell Url
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	int				nSiteScript;						//Shell 脚本类型

	bool			bData;								//保存的路劲有输入，就不用随之改变了
};
