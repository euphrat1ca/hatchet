#pragma once
#include <atlimage.h>
#include "panel.h"
#include "ImageButton.h"

class CCaptionRightPanel :
	public CPanel
{
public:
	CCaptionRightPanel(void);
	~CCaptionRightPanel(void);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnClickedMinBtn();
	afx_msg void OnClickedTrayBtn();
	afx_msg void OnClickedMaxBtn();
	afx_msg void OnClickedCloseBtn();
public:
	CImage m_imgCaption;
	CImageButton m_imgTrayBtn;
	CImageButton m_imgMinBtn;
	CImageButton m_imgMaxBtn;
	CImageButton m_imgCloseBtn;
};
