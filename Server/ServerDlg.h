// ServerDlg.h : header file
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "ServerControl.h"
#include <afxsock.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <ctime>

#ifndef SOCKET
#define PORT 8000
#endif // !SOCKET

// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = nullptr);	// standard constructor
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	ServerControl m_server;

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void OnOK();
	void OnCancel();

protected:
	CButton m_bnShare;
	CButton m_bnAdd;
	CButton m_bnDelete;
	BOOL m_isShared;

public:
	afx_msg void OnBnClickedButtonShare();
	afx_msg void OnBnClickedButtonClearHistory();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnLbnSetfocusListFiles();
	afx_msg void OnLbnKillfocusListFiles();

private:
	void hAddFile(CString filepath);
	void hAdjustScroll(CListBox* listbox);
};
