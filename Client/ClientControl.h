#pragma once

#include <functional>
#include <afxsock.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <ctime>

// ClientControl command target

class ClientControl : public CAsyncSocket
{
public:
	std::function<void()> GetFiles;
	std::function<void()> UserSignOut;
	CListBox* list;

	enum ANNOUNCE_TYPE
	{
		USER_SIGN_IN, USER_SIGN_UP, USER_SIGN_OUT, LIST_FILES, SHUT_DOWN
	};

public:
	ClientControl();
	virtual ~ClientControl();
	virtual void OnReceive(int nErrorCode);

private:
	void hAnnounceHandler(std::string announce);
	std::string hGetCurrentDate();
};


