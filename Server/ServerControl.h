#pragma once

// ServerControl command target

#include <afxsock.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <ctime>

class ServerControl : public CAsyncSocket
{
protected:
	enum REQUEST_TYPE
	{
		SIGN_UP, SIGN_IN, SIGN_OUT, DOWNLOAD, GET_LIST_FILE, GET_LIST_LOG
	};

	struct FileData
	{
		std::string folderpath;
		std::string filename;
	};

	struct Client
	{
		std::string username;
		std::string password;
		BOOL isSignIn;
		std::vector<std::string> log;
	};

	struct Request
	{
		REQUEST_TYPE type;
		std::string username;
		std::string password;
		std::string message;
	};

	std::vector<FileData> m_shared_files;
	std::vector<Client> m_client;
	std::vector<std::thread> m_thread;
	CListBox* m_listfiles;
	CListBox* m_listonline;

public:
	ServerControl();
	virtual ~ServerControl();

public:
	void AddSharedFile(std::string filepath);
	void DeleteSharedFile(std::string filename);
	void SetListFiles(CListBox* pListBox);
	void SetListOnline(CListBox* pListBox);

	// Overridable callbacks
public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void Close();

protected:
	Request hMsgToReq(std::string msg);
	void hRequestHandler(SOCKET hSocket);
	BOOL hSignUp(std::string username, std::string password, SOCKET socket);
	BOOL hSignIn(std::string username, std::string password, SOCKET socket);
	void hSignOut(std::string username);
	void hDownload(CSocket & connector, std::string username, std::string filename);
	void hSendListFile(CSocket & connector);
	void hSendListLog(CSocket & connector, std::string username);
	void hAdjustScroll(CListBox * listbox);
	void hUpdateOnline();
	std::string hGetCurrentDate();
};


