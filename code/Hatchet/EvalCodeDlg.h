#pragma once


// CEvalCodeDlg 对话框

class CEvalCodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CEvalCodeDlg)

public:
	CEvalCodeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEvalCodeDlg();

// 对话框数据
	enum { IDD = IDD_EVALCODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedEvalcodeImport();
	afx_msg void OnBnClickedEvalcodeExport();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CString			szSiteUrl;							//Shell Url
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	int				nSiteScript;						//Shell 脚本类型
	CString			szSiteJoinUrl;
	CString			szSiteJoinPass;

};
