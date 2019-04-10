
// Client.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifndef SOCKET
#define PORT 8000
#endif // !SOCKET


// CClientApp:
// See Client.cpp for the implementation of this class
//

class CClientApp : public CWinApp
{
public:
	CClientApp();

// Overrides
public:
	virtual BOOL InitInstance();
	CWnd* GetDlg();
// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CClientApp theApp;
