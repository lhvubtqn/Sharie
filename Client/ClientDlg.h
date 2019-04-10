
// ClientDlg.h : header file
//

#pragma once

#include <WinUser.h>
#include <afxsock.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>

#include <ctime>

// CClientDlg dialog
class CClientDlg : public CDialogEx
{
// Construction
public:
	CClientDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual void OnCancel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	enum REQUEST_TYPE
	{
		SIGN_UP, SIGN_IN, SIGN_OUT, DOWNLOAD, GET_LIST_FILE, GET_LIST_LOG
	};

	struct DownloadInfo
	{
		std::string state;
		std::string filename;
		std::string filepath;
	};

	BOOL m_isSignIn;
	std::string m_username;
	std::string m_password;
	CIPAddressCtrl m_serverIP;
	std::vector<std::thread> m_thread;
	std::vector<DownloadInfo> m_download_info;
	CListBox m_downloadList;

private:
	std::string hGetServerAddress();
	std::string hGetWindowText(int CWndId);
	void hUpdate();
	void hUserSignIn();
	void hUserSignOut();
	void hGetListFile();
	void hGetListLog();
	void hDownload(std::string filename);
	void hStartDownload(std::string filename, std::string filepath);
	void hFinishDownload(std::string filename, std::string filepath);
	void hDownloadPercent(std::string filename, std::string filepath, int percent);
	BOOL hConnectToServer(CSocket* connector);
	BOOL hSendRequest(REQUEST_TYPE type, CAsyncSocket* connector, std::string filename = "");
	void hAdjustScroll(CListBox* listbox);
	void OnOK() {};

public:
	afx_msg void OnBnClickedButtonSignIn();
	afx_msg void OnBnClickedButtonSignUp();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnLbnSetfocusListFiles();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonOpenFolder();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnLbnSetfocusListInfo();
	afx_msg void OnLbnKillfocusListInfo();
	afx_msg void OnBnClickedButtonOpenFile();

	friend class ClientControl;
};
