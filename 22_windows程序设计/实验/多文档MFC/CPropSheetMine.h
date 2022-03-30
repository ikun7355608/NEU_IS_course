#pragma once
#include "Resource.h"
#include "CPropPage1.h"
#include "CPropPage2.h"


// CPropSheetMine

class CPropSheetMine : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CPropSheetMine)
	CPropSheetMine::CPropSheetMine(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0)
		: CMFCPropertySheet(pszCaption, pParentWnd, iSelectPage)
	{
		AddPage(&m_propPage1);
		AddPage(&m_propPage2);
	}
public:
	CPropSheetMine();
	virtual ~CPropSheetMine();
	CPropPage1  m_propPage1;
	CPropPage2  m_propPage2;

protected:
	DECLARE_MESSAGE_MAP()
};


