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
		SIGN_UP, SIGN_IN, SIGN_OUT, DOWNLOAD, GET_LIST_FILE, CONNECT
	};

	enum ANNOUNCE_TYPE
	{
		USER_SIGN_IN, USER_SIGN_UP, USER_SIGN_OUT, LIST_FILES, SHUT_DOWN
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
		SOCKET socket;
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
	CListBox* m_listbox;

public:
	ServerControl();
	virtual ~ServerControl();

public:
	void AddSharedFile(std::string filepath);
	void DeleteSharedFile(std::string filename);
	void SetListBox(CListBox* pListBox);

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
	void hAnnounce(ANNOUNCE_TYPE type, std::string username = "");
	BOOL hSignUp(std::string username, std::string password, SOCKET socket);
	BOOL hSignIn(std::string username, std::string password, SOCKET socket);
	void hSignOut(std::string username);
	void hDownload(CSocket & connector, std::string username, std::string filename);
	void hSendListFile(CSocket & connector);
	void hAdjustScroll(CListBox * listbox);
	std::string hGetCurrentDate();
};


