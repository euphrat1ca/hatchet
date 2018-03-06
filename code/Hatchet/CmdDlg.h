#pragma once


// CCmdDlg 对话框

class CCmdDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdDlg)

public:
	CCmdDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCmdDlg();

// 对话框数据
	enum { IDD = IDD_CMD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

//protected:
public:
//	afx_msg void GetCmdData(CString szCmd);

	CEdit			m_cmd;
	CWnd			*pEdit;								//文本框句柄
	int				CharLen_Old;						//回车前的个数
	int				CharLen_New;						//最新的个数
	CString			Cmdtemp;
	CString			szCmdRoot;
	bool			bIsSetSel;							//会被全选
	CStringArray	szCmdDos;							//历史命令
	int				nCmdDosCont;						//计数，上下箭头
	CString			OldCmdtemp;							//用来保存以往的消息，一会儿向上向下时，需要到
	bool			IsWin;								//是否WIN系统 (/) 换成 (\)
	CString			WWWRoot;							//缓存技术，暂时还没有，有了，需要修改
	CString			szSystemInfo;						//[C:D:E:F:G:X:Y:	Windows NT QN 6.1 build 7601 (Windows 7 Ultimate Edition Service Pack 1) i586(SYSTEM)]
	CString			szSiteUrl;							//Shell Url
	CString			szSiteJoinUrl;
	CString			szSiteJoinPass;
	CString			szSitePass;							//Shell Pass
	int				nSiteCode;							//Shell Code 编码
	CString			szSiteCode;							//JSP 编码
	CString			szSiteConfig;						//Shell Config
	int				nSiteScript;						//Shell 脚本类型
	CString			szCmdFamatPageCode;					//用来格式化TEMP

	CString			szCacheItem;						//缓存技术
	CString			szCmdExeAddr;						//CMD位置

	bool			bThreadFinish;						//判断是否上条命令执行完毕
	CString			szCmd;								//线程里，要执行的CMD
public:
	afx_msg void OnEnKillfocusCmd();
};
