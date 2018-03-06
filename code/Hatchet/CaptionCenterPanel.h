#pragma once
#include "panel.h"

class CCaptionCenterPanel :
	public CPanel
{
public:
	CCaptionCenterPanel(void);
	~CCaptionCenterPanel(void);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	// 资源位图
    CBitmap m_bmpRes;
    // 资源设备
    CDC     ImageDC;
	afx_msg bool AddTab(CString szName, int nType);
	afx_msg void RemoveTab(int index);

protected:
	#define TB_CLR_TRAN     RGB(255, 0, 255)
	#define TB_MAX_TAB      30					//最多个数

	HICON	hIcon_Dir;
	HICON	hIcon_Cmd;
	HICON	hIcon_Data;
	HICON	hIcon_FileSave;
	HICON	hIcon_Proxy;
	HICON	hIcon_EvalCode;
	HICON	hIcon_Registry;
	HICON	hIcon_Browse;

	int		nHover;						//鼠标徘徊的个数
	int		nOldHover;					//鼠标徘徊的个数
	int		TabLeng;					//标签长度

	CString m_svTabNames[TB_MAX_TAB];	// 标签名
	DWORD   m_dwvTabDatas[TB_MAX_TAB];	// 标签数据
	int     m_iTabNum;					// 标签数
	int		m_nType[TB_MAX_TAB];		// 图标类型 1 2 3 4	
};

extern CCaptionCenterPanel*	pCaptionCenterPanel;