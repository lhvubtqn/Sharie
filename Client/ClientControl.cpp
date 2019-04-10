// ClientControl.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientControl.h"


// ClientControl

ClientControl::ClientControl() : CAsyncSocket::CAsyncSocket()
{
}

ClientControl::~ClientControl()
{
	CAsyncSocket::~CAsyncSocket();
}

void ClientControl::hAnnounceHandler(std::string announce)
{
	// Tách thông báo
	int type = std::stoi(announce.substr(0, announce.find_first_of('\n')));
	std::string username;
	if (announce.find_first_of('\n'))
		username = announce.substr(announce.find_first_of('\n') + 1, announce.length());

	// Lấy ngày hiện tại
	std::string datetime = hGetCurrentDate();

	// Xử lý
	switch (ANNOUNCE_TYPE(type))
	{
	case ANNOUNCE_TYPE::USER_SIGN_IN:
		list->AddString(CString((datetime + " " + username + " signed in.").c_str()));
		break;

	case ANNOUNCE_TYPE::USER_SIGN_UP:
		list->AddString(CString((datetime + " " + username + " signed up.").c_str()));
		break;

	case ANNOUNCE_TYPE::USER_SIGN_OUT:
		list->AddString(CString((datetime + " " + username + " signed out.").c_str()));
		break;

	case ANNOUNCE_TYPE::LIST_FILES:
		GetFiles();
		break;

	case ANNOUNCE_TYPE::SHUT_DOWN:
		AfxMessageBox(_T("Server is shutting down. Log out..."), MB_ICONWARNING | MB_OK, 0);
		UserSignOut();
		break;
	}
}

std::string ClientControl::hGetCurrentDate()
{
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "[%d-%m-%Y %H:%M:%S]", &timeinfo);
	std::string datetime(buffer);
	return datetime;
}

void ClientControl::OnReceive(int nErrorCode)
{
	// Nhận kích thước thông báo
	int msgSize = 0;
	this->Receive((char*)&msgSize, sizeof(int), 0);

	// Nhận thông báo
	char* msg = new char[msgSize + 1];
	this->Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';

	// Xử lý thông báo
	std::string announce(msg);
	if (announce != "")
		hAnnounceHandler(announce);
	CAsyncSocket::OnReceive(nErrorCode);
}


// ClientControl member functions
