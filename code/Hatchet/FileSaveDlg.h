#pragma once
#include "afxwin.h"


// CFileSaveDlg 对话框

class CFileSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileSaveDlg)

public:
	CFileSaveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileSaveDlg();

// 对话框数据
	enum { IDD = IDD_FILESAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnGetContent();
	afx_msg void OnSave();

//	afx_msg bool GetContent(CString szFilePath);

	afx_msg void OnEnSetfocusEditFilepath();
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	CEdit	m_FileSave_Edit_Find;
	CString szSiteUrl;
	CString szSitePass;
	CString szSiteFileSave;
	int		nSiteScript;
	int		nSiteCode;
	bool	IsWin;								//是否WIN系统 (/) 换成 (\)
	int		nFindPos;							//查找的开始位置
	CString szFindStr;							//查找的文本，内容控件里，需要比较

	bool			bThreadFinish;						//判断是否上条命令执行完毕
	CString			szFilePath;							//文件路径
};
