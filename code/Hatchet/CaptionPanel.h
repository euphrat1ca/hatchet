#pragma once
#include "panel.h"
#include "CaptionLeftPanel.h"
#include "CaptionCenterPanel.h"
#include "CaptionRightPanel.h"

class CCaptionPanel :
	public CPanel
{
public:
	CCaptionPanel(void);
	~CCaptionPanel(void);

private:
	CCaptionLeftPanel	m_CaptionLeftPanel;
	CCaptionCenterPanel	m_CaptionCenterPanel;
	CCaptionRightPanel	m_CaptionRightPanel;
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void UpdateSystemMenu(CMenu* pMenu);
};
