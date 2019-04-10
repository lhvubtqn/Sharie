// ServerControl.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerControl.h"

// ServerControl

ServerControl::ServerControl()
{
}

ServerControl::~ServerControl()
{
	for (int i = 0; i < m_thread.size(); ++i)
	{
		m_thread[i].join();
	}
}

void ServerControl::OnReceive(int nErrorCode)
{
}

void ServerControl::OnSend(int nErrorCode)
{
}

void ServerControl::OnOutOfBandData(int nErrorCode)
{
}

void ServerControl::OnAccept(int nErrorCode)
{
	CSocket connector;
	if (Accept(connector))
	{
		SOCKET hSocket = connector.Detach();
		m_thread.push_back(std::thread(&ServerControl::hRequestHandler, this, hSocket));
	}
	CAsyncSocket::OnAccept(nErrorCode);
}

void ServerControl::OnConnect(int nErrorCode)
{
}

void ServerControl::OnClose(int nErrorCode)
{
}

void ServerControl::Close()
{
	m_listonline->ResetContent();
	std::string datetime = hGetCurrentDate();
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].isSignIn)
		{
			this->m_listfiles->AddString(CString((datetime + " " + m_client[i].username + " signed out.").c_str()));
			m_client[i].log.clear();
			m_client[i].isSignIn = FALSE;
		}
	}
	CAsyncSocket::Close();
}

ServerControl::Request ServerControl::hMsgToReq(std::string msg)
{
	Request req;
	// Loại request
	int split = msg.find_first_of('\n');
	req.type = REQUEST_TYPE(std::stoi(msg.substr(0, split)));

	// Username
	req.username = msg.substr(split + 1, msg.find_first_of('\n', split + 1) - split - 1);
	split = msg.find_first_of('\n', split + 1);

	// Password
	req.password = msg.substr(split + 1, msg.find_first_of('\n', split + 1) - split - 1);
	split = msg.find_first_of('\n', split + 1);

	// Nếu là request download, còn có thêm tên tệp cần download
	if (msg.find_first_of('\n', split + 1) != msg.npos)
		req.message = msg.substr(split + 1, msg.find_first_of('\n', split + 1) - split - 1);
	return req;
}

void ServerControl::AddSharedFile(std::string filepath)
{
	FileData file;

	if (filepath.back() == '\\')
		filepath.erase(filepath.end());

	file.filename = filepath.substr(filepath.find_last_of('\\') + 1, filepath.length());
	file.folderpath = filepath.substr(0, filepath.find_last_of('\\') + 1);

	m_shared_files.push_back(file);
}

void ServerControl::DeleteSharedFile(std::string filename)
{
	int i = 0;
	while (m_shared_files[i].filename != filename)
		++i;
	if (i < m_shared_files.size())
		m_shared_files.erase(m_shared_files.begin() + i);
}

void ServerControl::SetListFiles(CListBox * pListBox)
{
	m_listfiles = pListBox;
}

void ServerControl::SetListOnline(CListBox * pListBox)
{
	m_listonline = pListBox;
}

void ServerControl::hRequestHandler(SOCKET hSocket)
{
	CSocket connector;
	connector.Attach(hSocket);

	// Nhận về kích thước message
	int msgSize = 0;
	char* msg;
	connector.Receive((char*)&msgSize, sizeof(int), 0);
	if (msgSize == 0)
		return;

	// Nhận message
	msg = new char[msgSize];
	connector.Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';

	// Phân tích request
	Request req = hMsgToReq(std::string(msg));

	switch (req.type)
	{
	case REQUEST_TYPE::SIGN_UP:
		if (hSignUp(req.username, req.password, hSocket))
			msg = "1\n";
		else
			msg = "0\n";
		connector.Send(msg, strlen(msg), 0);
		break;

	case REQUEST_TYPE::SIGN_IN:
		if (hSignIn(req.username, req.password, hSocket))
			msg = "1\n";
		else
			msg = "0\n";
		connector.Send(msg, strlen(msg), 0);
		break;

	case REQUEST_TYPE::SIGN_OUT:
		hSignOut(req.username);
		break;

	case REQUEST_TYPE::DOWNLOAD:
		hDownload(connector, req.username, req.message);
		break;

	case REQUEST_TYPE::GET_LIST_FILE:
		hSendListFile(connector);
		break;

	case REQUEST_TYPE::GET_LIST_LOG:
		hSendListLog(connector, req.username);
		break;
	}

	// Đóng kết nối
	connector.Detach();
}

BOOL ServerControl::hSignUp(std::string username, std::string password, SOCKET socket)
{
	// Kiểm tra xem đã tồn tại chưa
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].username == username)
			return FALSE;
	}

	// Nếu chưa tồn tại, thêm vào danh sách client
	m_client.push_back(Client{ username, password, TRUE });
	
	// Thông báo trên history
	std::string datetime = hGetCurrentDate();
	this->m_listfiles->AddString(CString((datetime + " " + username + " signed up.").c_str()));

	// Update danh sách client online
	hUpdateOnline();

	// Thông báo các client khác
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].isSignIn && m_client[i].username != username)
			m_client[i].log.push_back(datetime + " " + username + " signed up.");
	}

	// Điều chỉnh thanh cuộn
	hAdjustScroll(m_listfiles);
	return TRUE;
}

BOOL ServerControl::hSignIn(std::string username, std::string password, SOCKET socket)
{
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].username == username && m_client[i].password == password)
		{
			// Đang đăng nhập
			m_client[i].isSignIn = TRUE;

			// Thông báo trên history
			std::string datetime = hGetCurrentDate();
			this->m_listfiles->AddString(CString((datetime + " " + username + " signed in.").c_str()));

			// Update danh sách client online
			hUpdateOnline();

			// Thông báo các client khác
			for (int i = 0; i < m_client.size(); ++i)
			{
				if (m_client[i].isSignIn && m_client[i].username != username)
					m_client[i].log.push_back(datetime + " " + username + " signed in.");
			}

			// Điều chỉnh thanh cuộn
			hAdjustScroll(m_listfiles);
			return TRUE;
		}
	}
	return FALSE;
}

void ServerControl::hSignOut(std::string username)
{
	// Đăng xuất
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].username == username)
		{
			m_client[i].isSignIn = FALSE;
			m_client[i].log.clear();
			break;
		}
	}

	// Thông báo trên history
	std::string datetime = hGetCurrentDate();
	this->m_listfiles->AddString(CString((datetime + " " + username + " signed out.").c_str()));

	// Update danh sách client online
	hUpdateOnline();

	// Thông báo các client khác
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].isSignIn && m_client[i].username != username)
			m_client[i].log.push_back(datetime + " " + username + " signed out.");
	}

	// Điều chỉnh thanh cuộn
	hAdjustScroll(m_listfiles);
}

void ServerControl::hDownload(CSocket & connector, std::string username, std::string filename)
{
	std::string filepath;
	for (int i = 0; i < m_shared_files.size(); ++i)
	{
		if (m_shared_files[i].filename == filename)
		{
			filepath = m_shared_files[i].folderpath + filename;
			break;
		}
	}

	// Mở file
	FILE* file;
	fopen_s(&file, filepath.c_str(), "rb");

	// Nếu không có file, thông báo tải không thành công
	if (file == nullptr)
	{
		char* msg = "0\n";
		connector.Send((char*)msg, strlen(msg), 0);
		return;
	}

	// Thông báo tải được
	char* msg = "1\n";
	connector.Send((char*)msg, strlen(msg), 0);

	// Gửi kích thước file
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	connector.Send((char*)&size, sizeof(int), 0);

	// Gửi kích thước block
	int block = 1024;
	connector.Send((char*)&block, sizeof(int), 0);

	// Gửi file
	msg = new char[block];
	while (true)
	{
		// Đếm số byte đọc được và gửi đi
		int msgSize = fread(msg, 1, block, file);
		connector.Send((char*)msg, msgSize, 0);

		// Nếu hết file
		if (msgSize < block) break;
	}
	fclose(file);

	// Nhận kết quả
	connector.Receive((char*)msg, 2, 0);
	msg[2] = '\0';

	// Nếu thành công
	if (msg[0] == '1')
	{
		// Thông báo lên lịch sử
		std::string datetime = hGetCurrentDate();
		this->m_listfiles->AddString(CString((datetime + " " + username + " downloaded " + filename + ".").c_str()));
		hAdjustScroll(m_listfiles);
	}

	// Delete msg
	if (msg) delete[] msg;
}

void ServerControl::hSendListFile(CSocket & connector)
{
	std::string list(std::to_string(m_shared_files.size()) + "\n");
	for (int i = 0; i < m_shared_files.size(); ++i)
	{
		list += m_shared_files[i].filename + "\n";
	}

	int size = list.length();
	connector.Send((char*)&size, sizeof(int), 0);
	connector.Send((char*)list.c_str(), size, 0);
}

void ServerControl::hSendListLog(CSocket & connector, std::string username)
{
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].username == username)
		{
			// Gửi list hiện tại
			std::string list(std::to_string(m_client[i].log.size()) + "\n");
			for (int j = 0; j < m_client[i].log.size(); ++j)
			{
				list += m_client[i].log[j] + "\n";
			}

			int size = list.length();
			connector.Send((char*)&size, sizeof(int), 0);
			connector.Send((char*)list.c_str(), size, 0);
			break;
		}
	}
}

void ServerControl::hAdjustScroll(CListBox * listbox)
{
	CString str;
	CSize size;
	int dx = 0;
	CDC* pDC = listbox->GetDC();
	for (int i = 0; i < listbox->GetCount(); i++)
	{
		listbox->GetText(i, str);
		size = pDC->GetTextExtent(str);

		// Tìm dòng có chiều dài theo px dài nhất
		if (size.cx > dx)
			dx = size.cx;
	}
	listbox->ReleaseDC(pDC);

	// Thay đổi thanh cuộn ngang cho phù hợp
	listbox->SetHorizontalExtent(dx);
	ASSERT(listbox->GetHorizontalExtent() == dx);
}
void ServerControl::hUpdateOnline()
{
	m_listonline->ResetContent();
	for (int i = 0; i < m_client.size(); ++i)
	{
		if (m_client[i].isSignIn)
		{
			m_listonline->AddString(CString(m_client[i].username.c_str()));
		}
	}
}
std::string ServerControl::hGetCurrentDate()
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
// ServerControl member functions
