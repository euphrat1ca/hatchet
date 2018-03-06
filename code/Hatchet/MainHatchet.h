#pragma once


#include <atlimage.h>
#include "CaptionPanel.h"
// CMainHatchet

class CMainHatchet : public CWnd
{
	DECLARE_DYNAMIC(CMainHatchet)

public:
	CMainHatchet();
	virtual ~CMainHatchet();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();

	afx_msg void AddFileTab();
	afx_msg void AddCmdTab();
	afx_msg void AddDataTab();
	afx_msg void AddFileSaveTab();
	afx_msg void AddProxyTab();
	afx_msg void AddEvalCodeTab();
	afx_msg void AddRegistryTab();
	afx_msg void AddBrowseTab();

	
	afx_msg void SelchangeTab(int n=-1);	//-1 显示Main，从0开始 0=第一个
	afx_msg void DeleteTab(int n=-1);		//-1 删除最后一个
	afx_msg void OnMyTray();		//托盘

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);//托盘

	int nOldSelectPos;				//之前选择的标签，记忆选择。文件管理，文件内容管理，内容关闭，回到文件管理
	int nNewSelectPos;				//之前选择的标签，记忆选择。文件管理，文件内容管理，内容关闭，回到文件管理
public:
	CImage m_imgBorder;
	CCaptionPanel m_captionPanel;
	CPtrList m_PtrAllDlg;

	NOTIFYICONDATA NotifyIcon;			//托盘
};


