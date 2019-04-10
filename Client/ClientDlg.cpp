
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg dialog



CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
}

void CClientDlg::OnCancel()
{
	if (m_isSignIn)
	{
		m_isSignIn = FALSE;
		CSocket connector;
		hConnectToServer(&connector);
		hSendRequest(REQUEST_TYPE::SIGN_OUT, &connector);
	}

	for (int i = 0; i < m_thread.size(); ++i)
	{
		// m_thread[i].join();
	}
	CDialogEx::OnCancel();
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_serverIP);
	DDX_Control(pDX, IDC_LIST_FILES, m_downloadList);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SIGN_IN, &CClientDlg::OnBnClickedButtonSignIn)
	ON_BN_CLICKED(IDC_BUTTON_SIGN_UP, &CClientDlg::OnBnClickedButtonSignUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CClientDlg::OnBnClickedButtonDownload)
	ON_LBN_SETFOCUS(IDC_LIST_FILES, &CClientDlg::OnLbnSetfocusListFiles)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CClientDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FOLDER, &CClientDlg::OnBnClickedButtonOpenFolder)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CClientDlg::OnBnClickedButtonClear)
	ON_LBN_SETFOCUS(IDC_LIST_INFO, &CClientDlg::OnLbnSetfocusListInfo)
	ON_LBN_KILLFOCUS(IDC_LIST_INFO, &CClientDlg::OnLbnKillfocusListInfo)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE, &CClientDlg::OnBnClickedButtonOpenFile)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Khởi tạo thư viện Socket
	if (AfxSocketInit() == FALSE)
	{
		throw "AfxSocketInit Error!";
	}

	// IP mặc định của server
	m_serverIP.SetAddress(127, 0, 0, 1);

	// Chưa đăng nhập
	m_isSignIn = FALSE;

	// Đóng download
	CWnd* download_bn = this->GetDlgItem(IDC_BUTTON_DOWNLOAD);
	download_bn->EnableWindow(FALSE);
	CWnd* refresh_bn = this->GetDlgItem(IDC_BUTTON_REFRESH);
	refresh_bn->EnableWindow(FALSE);
	CWnd* download_list = this->GetDlgItem(IDC_LIST_FILES);
	download_list->EnableWindow(FALSE);
	((CListBox*)download_list)->ResetContent();

	// Đóng log
	CWnd* log_list = this->GetDlgItem(IDC_LIST_LOG);
	log_list->EnableWindow(FALSE);

	// Chưa có file, không mở file được
	CWnd* bn_folder = this->GetDlgItem(IDC_BUTTON_OPEN_FOLDER);
	CWnd* bn_file = this->GetDlgItem(IDC_BUTTON_OPEN_FILE);
	bn_folder->EnableWindow(FALSE);
	bn_file->EnableWindow(FALSE);

	// Lấy đối tượng có ID = IDC_EDIT_PASSWORD
	// Và chuyển password thành các ký tự *
	CEdit* pass = (CEdit*)this->GetDlgItem(IDC_EDIT_PASSWORD);
	pass->SetPasswordChar('*');

	// Khởi tạo mở file
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::string CClientDlg::hGetServerAddress()
{
	BYTE byte1, byte2, byte3, byte4;
	m_serverIP.GetAddress(byte1, byte2, byte3, byte4);
	std::string address = std::to_string(byte1) + '.' + std::to_string(byte2)
		+ '.' + std::to_string(byte3) + '.' + std::to_string(byte4);
	return address;
}

std::string CClientDlg::hGetWindowText(int CWndId)
{
	CWnd* edit = this->GetDlgItem(CWndId);
	CString text;
	edit->GetWindowTextW(text);

	CT2CA tmp(text);
	return std::string(tmp);
}

void CClientDlg::hUpdate()
{
	// Mỗi 500ms update một lần
	while (m_isSignIn)
	{
		Sleep(500);
		if (!m_isSignIn) return;

		CSocket connector;
		connector.Create();
		if (connector.Connect(CString(hGetServerAddress().c_str()), PORT) == FALSE)
		{
			MessageBoxA(this->m_hWnd, "Connection lost!", NULL, MB_ICONWARNING | MB_OK);
			hUserSignOut();
			return;
		}
		connector.ShutDown();
		connector.Close();

		hGetListFile();
		hGetListLog();
	}
}

void CClientDlg::hUserSignIn()
{
	// Mở Update
	m_isSignIn = TRUE;
	m_thread.push_back(std::thread(&CClientDlg::hUpdate, this));

	// Không cho người dùng đăng ký 
	CWnd* sign_up = this->GetDlgItem(IDC_BUTTON_SIGN_UP);
	sign_up->EnableWindow(FALSE);

	// Chuyển đăng nhập sang đăng xuất
	CWnd* sign_in = this->GetDlgItem(IDC_BUTTON_SIGN_IN);
	sign_in->SetWindowTextW(_T("Sign Out"));

	// Đóng hộp đăng nhập
	CWnd* address = this->GetDlgItem(IDC_IPADDRESS);
	address->EnableWindow(FALSE);
	CWnd* username = this->GetDlgItem(IDC_EDIT_USERNAME);
	username->EnableWindow(FALSE);
	CWnd* password = this->GetDlgItem(IDC_EDIT_PASSWORD);
	password->EnableWindow(FALSE);

	// Mở download
	CWnd* download_bn = this->GetDlgItem(IDC_BUTTON_DOWNLOAD);
	download_bn->EnableWindow(FALSE);
	CWnd* refresh_bn = this->GetDlgItem(IDC_BUTTON_REFRESH);
	refresh_bn->EnableWindow(TRUE);
	CWnd* download_list = this->GetDlgItem(IDC_LIST_FILES);
	download_list->EnableWindow(TRUE);

	// Mở log
	CWnd* log_list = this->GetDlgItem(IDC_LIST_LOG);
	log_list->EnableWindow(TRUE);

	// Lấy tập các tên tệp
	hGetListFile();

	// Điều chỉnh download_list
	hAdjustScroll((CListBox*)download_list);
}

void CClientDlg::hUserSignOut()
{
	// Cho người dùng đăng ký 
	CWnd* sign_up = this->GetDlgItem(IDC_BUTTON_SIGN_UP);
	sign_up->EnableWindow(TRUE);

	// Chuyển đăng xuất sang đăng nhập
	m_isSignIn = FALSE;
	CWnd* sign_in = this->GetDlgItem(IDC_BUTTON_SIGN_IN);
	sign_in->SetWindowTextW(_T("Sign In"));

	// Mở lại hộp đăng nhập
	CWnd* address = this->GetDlgItem(IDC_IPADDRESS);
	address->EnableWindow(TRUE);
	CWnd* username = this->GetDlgItem(IDC_EDIT_USERNAME);
	username->EnableWindow(TRUE);
	CWnd* password = this->GetDlgItem(IDC_EDIT_PASSWORD);
	password->EnableWindow(TRUE);

	// Đóng download
	CWnd* download_bn = this->GetDlgItem(IDC_BUTTON_DOWNLOAD);
	download_bn->EnableWindow(FALSE);
	CWnd* refresh_bn = this->GetDlgItem(IDC_BUTTON_REFRESH);
	refresh_bn->EnableWindow(FALSE);
	CWnd* download_list = this->GetDlgItem(IDC_LIST_FILES);
	download_list->EnableWindow(FALSE);
	((CListBox*)download_list)->ResetContent();
	hAdjustScroll((CListBox*)download_list);

	// Xóa log
	CWnd* log_list = this->GetDlgItem(IDC_LIST_LOG);
	log_list->EnableWindow(FALSE);
	((CListBox*)log_list)->ResetContent();
}

void CClientDlg::hGetListFile()
{
	// Lấy con trỏ list
	CListBox* list = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);

	// Connect to server
	CSocket connector;
	if (!hConnectToServer(&connector)) return;

	// Gửi request lấy list files
	hSendRequest(REQUEST_TYPE::GET_LIST_FILE, &connector);

	// Nhận kích thước msg
	int msgSize = 0;
	connector.Receive((char*)&msgSize, sizeof(int), 0);

	// Nhận msg
	char* msg = new char[msgSize + 1];
	connector.Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';

	// Lấy kích thước mảng tên tệp
	std::string sMsg(msg);
	int start = sMsg.find_first_of('\n');
	int size = std::stoi(sMsg.substr(0, start));

	// Lấy list file
	std::vector<std::string> list_files;
	for (int i = 0; i < size; ++i)
	{
		std::string filename = sMsg.substr(start + 1, sMsg.find_first_of('\n', start + 1) - start - 1);
		list_files.push_back(filename);
		start = sMsg.find_first_of('\n', start + 1);
	}

	// Thêm những file chưa có
	for (int i = 0; i < list_files.size(); ++i)
	{
		CString filename(list_files[i].c_str());
		if (list->FindString(0, filename) == LB_ERR)
		{
			list->AddString(filename);
			list->SetTopIndex(list->GetCount() - 1);
		}
	}

	// Xóa những file không có trong list_files
	int index = 0;
	while (index < list->GetCount())
	{
		BOOL exist = FALSE;
		for (int i = 0; i < list_files.size(); ++i)
		{
			CString filename(list_files[i].c_str());
			CString compare;
			list->GetText(index, compare);
			if (compare == filename) exist = TRUE;
		}
		if (exist) index++;
		else
		{
			list->DeleteString(index);
			list->SetTopIndex(list->GetCount() - 1);
		}
	}
	hAdjustScroll(list);

	// Delete msg
	if (msg) delete[]msg;

	// Đóng kết nối
	connector.ShutDown(2);
	connector.Close();
}

void CClientDlg::hGetListLog()
{
	// Lấy pointer
	CListBox* list = (CListBox*)this->GetDlgItem(IDC_LIST_LOG);

	// Connect to server
	CSocket connector;
	if (!hConnectToServer(&connector)) return;

	// Gửi request lấy list log
	hSendRequest(REQUEST_TYPE::GET_LIST_LOG, &connector);

	// Nhận kích thước msg
	int msgSize = 0;
	connector.Receive((char*)&msgSize, sizeof(int), 0);

	// Nhận msg
	char* msg = new char[msgSize + 1];
	connector.Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';

	// Lấy kích thước mảng
	std::string sMsg(msg);
	int start = sMsg.find_first_of('\n');
	int size = std::stoi(sMsg.substr(0, start));

	// Thêm vào list file
	for (int i = 0; i < size; ++i)
	{
		std::string log = sMsg.substr(start + 1, sMsg.find_first_of('\n', start + 1) - start - 1);
		CString item = CString(log.c_str());
		if (list->FindString(0, item) == LB_ERR)
		{
			list->AddString(item);
			list->SetTopIndex(list->GetCount() - 1);
		}	
		start = sMsg.find_first_of('\n', start + 1);
	}

	// Delete msg
	if (msg) delete[]msg;

	// Đóng kết nối
	connector.ShutDown(2);
	connector.Close();
}

void CClientDlg::hDownload(std::string filename)
{
	// Connect to server
	CSocket connector;
	if (!hConnectToServer(&connector)) return;

	// Gửi request download
	hSendRequest(REQUEST_TYPE::DOWNLOAD, &connector, filename);

	// Kiểm tra xem có lỗi không
	int msgSize = 2;
	char* msg = new char[msgSize + 1];
	connector.Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';
	if (msg[0] == '0')
	{
		AfxMessageBox(_T("Download failed!"), MB_ICONSTOP | MB_OK, 0);
		return;
	}
	delete msg;

	// Chọn nơi lưu
	std::string filepath;
	CFileDialog savefilename(FALSE, NULL, CString(filename.c_str()), OFN_NODEREFERENCELINKS | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);

	// Mở hộp thoại chọn file
	int act = savefilename.DoModal();

	// Cho người dùng bấm
	CWnd* bn = this->GetDlgItem(IDC_BUTTON_DOWNLOAD);
	bn->EnableWindow(TRUE);

	// Nếu người dùng cancel thì thoát
	if (act == IDCANCEL) return;

	// Ngược lại lấy tên đường dẫn tệp
	CString path = savefilename.GetPathName();
	CT2CA tmp(path);
	filepath = tmp;

	// Thêm vào danh sách download
	hStartDownload(filename, filepath);

	// Bắt đầu nhận dữ liệu
	FILE* file;
	fopen_s(&file, filepath.c_str(), "wb");

	// Nhận kích thước file
	int file_size = 0;
	connector.Receive((char*)&file_size, sizeof(int), 0);

	// Nhận kích thước block
	int block = 0;
	connector.Receive((char*)&block, sizeof(int), 0);

	// Nhận dữ liệu
	msg = new char[block + 1];
	int remain = file_size;
	int old_percent = 0;
	while (m_isSignIn)
	{
		// Nhận một block
		msgSize = connector.Receive((char*)msg, block, 0);
		msg[msgSize] = '\0';
		msgSize = min(msgSize, remain);
		remain -= msgSize;
		
		// Ghi vào tệp
		fwrite(msg, 1, msgSize, file);

		// Thông báo đã tải được bao nhiêu
		int new_percent = ((file_size - remain) * 1.0 / file_size) * 100;
		if (new_percent != old_percent)
		{
			hDownloadPercent(filename, filepath, new_percent);
			old_percent = new_percent;
		}

		// Nếu đủ dữ liệu, thoát
		if (remain == 0) break;
	}
	fclose(file);

	// Báo kết quả download ở information box
	hFinishDownload(filename, filepath);

	// Delete msg
	if (msg) delete[]msg;

	// Thông báo tải thành công
	msg = "1/n";
	connector.Send((char*)msg, 2, 0);

	// Đóng kết nối
	connector.ShutDown(2);
	connector.Close();
	return;
}

void CClientDlg::hStartDownload(std::string filename, std::string filepath)
{
	// Thêm vào danh sách download
	DownloadInfo dInfo;
	dInfo.filepath = filepath;
	dInfo.filename = filename;
	dInfo.state = "Downloading__0%__";
	m_download_info.push_back(dInfo);

	// Tách local file name từ file path
	std::string file = filepath.substr(filepath.find_last_of('\\') + 1, filepath.length());

	// Thêm vào list info
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	list_info->AddString(CString((dInfo.state + file).c_str()));

	// Điều chỉnh info box
	hAdjustScroll((CListBox*)list_info);
}

void CClientDlg::hFinishDownload(std::string filename, std::string filepath)
{
	CString old_info;
	CString new_info;
	for (int i = 0; i < m_download_info.size(); ++i)
	{
		if (filename == m_download_info[i].filename && filepath == m_download_info[i].filepath 
			&& m_download_info[i].state.substr(0, 11) == "Downloading")
		{
			// Lấy tên lưu file
			std::string file = filepath.substr(filepath.find_last_of('\\') + 1, filepath.length());

			// Lấy info cũ
			old_info = CString((m_download_info[i].state + file).c_str());

			// Đổi trạng thái
			if (m_download_info[i].state.substr(0, 19) == "Downloading__100%__")
				m_download_info[i].state = "Completed_____";
			else
				m_download_info[i].state = "Crashed_____";
			new_info = CString((m_download_info[i].state + file).c_str());
			break;
		}
	}

	// Đổi info
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	UINT index = list_info->FindString(0, old_info);
	list_info->DeleteString(index);
	index = list_info->AddString(new_info);
	list_info->SetCurSel(index);

	// Mở button mở file
	CWnd* bn_folder = this->GetDlgItem(IDC_BUTTON_OPEN_FOLDER);
	CWnd* bn_file = this->GetDlgItem(IDC_BUTTON_OPEN_FILE);
	bn_folder->EnableWindow(TRUE);
	bn_file->EnableWindow(TRUE);
}

void CClientDlg::hDownloadPercent(std::string filename, std::string filepath, int percent)
{
	CString old_info;
	CString new_info;
	for (int i = 0; i < m_download_info.size(); ++i)
	{
		if (filename == m_download_info[i].filename && filepath == m_download_info[i].filepath
			&& m_download_info[i].state.substr(0, 11) == "Downloading")
		{
			// Lấy tên lưu file
			std::string file = filepath.substr(filepath.find_last_of('\\') + 1, filepath.length());

			// Lấy info cũ
			old_info = CString((m_download_info[i].state + file).c_str());
			m_download_info[i].state = "Downloading__" + std::to_string(percent) + "%__";
			new_info = CString((m_download_info[i].state + file).c_str());
			break;
		}
	}

	// Đổi info
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	UINT index = list_info->FindString(0, old_info);
	list_info->DeleteString(index);
	index = list_info->AddString(new_info);
}

BOOL CClientDlg::hConnectToServer(CSocket * connector)
{
	// Địa chỉ server
	std::string address = hGetServerAddress();

	// Kết nối server
	connector->Create();
	if (connector->Connect(CString(address.c_str()), PORT) == FALSE)
	{
		AfxMessageBox(_T("Unable to connect to server. Please try again!"), MB_ICONSTOP | MB_OK, 0);
		if (m_isSignIn) hUserSignOut();
		return FALSE;
	}
	return TRUE;
}

BOOL CClientDlg::hSendRequest(REQUEST_TYPE type, CAsyncSocket * connector, std::string filename)
{
	std::string username = hGetWindowText(IDC_EDIT_USERNAME);
	std::string password = hGetWindowText(IDC_EDIT_PASSWORD);
	std::string request;
	int msgSize;

	request = std::to_string(type) + '\n' + username + '\n' + password + '\n' + filename + '\n';
	msgSize = request.length();
	connector->Send(&msgSize, sizeof(int), 0);
	connector->Send(request.c_str(), msgSize, 0);
	return TRUE;
}

void CClientDlg::hAdjustScroll(CListBox * listbox)
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

void CClientDlg::OnBnClickedButtonSignIn()
{
	// Tạo kết nối
	CSocket connector;
	if (!hConnectToServer(&connector))
		return;

	// Nếu đang đăng nhập thì đăng xuất
	if (m_isSignIn)
	{
		// Gửi request sign out
		hSendRequest(REQUEST_TYPE::SIGN_OUT, &connector);
		AfxMessageBox(_T("Sign out successfully!"), MB_ICONINFORMATION | MB_OK, 0);

		// UI khi đăng xuất
		hUserSignOut();
		return;
	}

	// Ngược lại đăng nhập
	// Gửi request đăng nhập
	hSendRequest(REQUEST_TYPE::SIGN_IN, &connector);
	
	// Nhận phản hồi
	int msgSize = 2;
	char* msg = new char[msgSize + 1];
	connector.Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';

	// Đăng nhập thành công hay không
	if (msg[0] == '0')
	{
		AfxMessageBox(_T("The username or password is incorrect!"), MB_ICONSTOP | MB_OK, 0);
	}
	else
	{
		AfxMessageBox(_T("Sign in successfully!"), MB_ICONINFORMATION | MB_OK, 0);
		// UI khi đăng nhập
		hUserSignIn();
	}

	// Delete msg
	if (msg) delete[]msg;
}


void CClientDlg::OnBnClickedButtonSignUp()
{
	// Tạo kết nối
	CSocket connector;
	if (!hConnectToServer(&connector))
		return;

	// Tạo request
	int type = REQUEST_TYPE::SIGN_UP;
	std::string username = hGetWindowText(IDC_EDIT_USERNAME);
	std::string password = hGetWindowText(IDC_EDIT_PASSWORD);

	// Gửi request sign up
	hSendRequest(REQUEST_TYPE::SIGN_UP, &connector);

	// Nhận phản hồi
	int msgSize = 2;
	char* msg = new char[msgSize + 1];
	connector.Receive((char*)msg, msgSize, 0);
	msg[msgSize] = '\0';

	// Đăng ký thành công hay không
	if (msg[0] == '0')
	{
		AfxMessageBox(_T("The username already exists. Please use a different username!"), MB_ICONSTOP | MB_OK, 0);
	}
	else
	{
		AfxMessageBox(_T("Sign up successfully!"), MB_ICONINFORMATION | MB_OK, 0);
		// UI khi đăng nhập
		hUserSignIn();
	}

	// Delete msg
	if (msg) delete[]msg;
}


void CClientDlg::OnBnClickedButtonDownload()
{
	// Ngăn không cho người dùng bấm nữa
	CWnd* bn = this->GetDlgItem(IDC_BUTTON_DOWNLOAD);
	bn->EnableWindow(FALSE);

	// Kiểm tra xem có tệp nào đang được chọn không
	CListBox* list = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);
	UINT uiSelect = list->GetCurSel();
	if (uiSelect == LB_ERR)
		return;

	// Lấy tên tệp
	CString item;
	list->GetText(uiSelect, item);
	CT2CA tmp(item);
	std::string filename(tmp);

	// Kiểm tra xem có đang download không
	for (int i = 0; i < m_download_info.size(); ++i)
	{
		if (m_download_info[i].filename == filename)
		{
			if (m_download_info[i].state.substr(0, 11) == "Downloading")
			{
				AfxMessageBox(_T("File is being downloading!"), MB_ICONSTOP | MB_OK, 0);
				return;
			}
			else
			{
				int act = AfxMessageBox(_T("You have already downloaded this file. Continue?"), MB_ICONEXCLAMATION | MB_YESNO, 0);
				if (act == IDNO)
				{
					return;
				}
			}
			break;
		}
	}

	// Tạo socket mới và download
	m_thread.push_back(std::thread(&CClientDlg::hDownload, this, filename));
}


void CClientDlg::OnLbnSetfocusListFiles()
{
	CWnd* download_bn = this->GetDlgItem(IDC_BUTTON_DOWNLOAD);
	if (m_downloadList.GetCount() > 0)
		download_bn->EnableWindow(TRUE);
	else
		download_bn->EnableWindow(FALSE);
}

void CClientDlg::OnBnClickedButtonRefresh()
{
	CListBox* list = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);
	list->ResetContent();
	hGetListFile();
}


void CClientDlg::OnBnClickedButtonOpenFolder()
{
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	// Kiểm tra xem có file nào đang được chọn không
	UINT index = list_info->GetCurSel();
	if (index != LB_ERR)
	{
		// Lấy tên file
		CString item;
		list_info->GetText(index, item);
		CT2CA tmp(item);
		std::string filepath(tmp);

		// Lấy đường dẫn đến file
		std::string filename = filepath.substr(14, filepath.length());
		for (int i = 0; i < m_download_info.size(); ++i)
		{
			if (filename == m_download_info[i].filename)
			{
				std::string filepath = m_download_info[i].filepath;

				// Mở folder và chọn file
				ITEMIDLIST *pidl = ILCreateFromPath(CString(filepath.c_str()));
				if (pidl) {
					SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
					ILFree(pidl);
				}
			}
		}
	}
}


void CClientDlg::OnBnClickedButtonClear()
{
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	list_info->ResetContent();

	// Hết file, không mở file được
	CWnd* bn_folder = this->GetDlgItem(IDC_BUTTON_OPEN_FOLDER);
	CWnd* bn_file = this->GetDlgItem(IDC_BUTTON_OPEN_FILE);
	bn_folder->EnableWindow(FALSE);
	bn_file->EnableWindow(FALSE);

	// Điều chỉnh info box
	hAdjustScroll((CListBox*)list_info);
}


void CClientDlg::OnLbnSetfocusListInfo()
{
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	CWnd* bn_folder = this->GetDlgItem(IDC_BUTTON_OPEN_FOLDER);
	CWnd* bn_file = this->GetDlgItem(IDC_BUTTON_OPEN_FILE);

	if (list_info->GetCount() > 0)
	{
		bn_folder->EnableWindow(TRUE);
		bn_file->EnableWindow(TRUE);
	}
	else
	{
		bn_folder->EnableWindow(FALSE);
		bn_file->EnableWindow(FALSE);
	}
}


void CClientDlg::OnLbnKillfocusListInfo()
{
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	CWnd* bn_folder = this->GetDlgItem(IDC_BUTTON_OPEN_FOLDER);
	CWnd* bn_file = this->GetDlgItem(IDC_BUTTON_OPEN_FILE);

	if (list_info->GetCount() > 0)
	{
		UINT index = list_info->GetCurSel();
		if (index == LB_ERR)
		{
			bn_folder->EnableWindow(FALSE);
			bn_file->EnableWindow(FALSE);
		}
		else
		{
			bn_folder->EnableWindow(TRUE);
			bn_file->EnableWindow(TRUE);
		}
	}
	else
	{
		bn_folder->EnableWindow(FALSE);
		bn_file->EnableWindow(FALSE);
	}
}


void CClientDlg::OnBnClickedButtonOpenFile()
{
	CListBox* list_info = (CListBox*)this->GetDlgItem(IDC_LIST_INFO);
	// Kiểm tra xem có file nào đang được chọn không
	UINT index = list_info->GetCurSel();
	if (index != LB_ERR)
	{
		// Lấy tên file
		CString item;
		list_info->GetText(index, item);
		CT2CA tmp(item);
		std::string filepath(tmp);

		// Lấy state
		std::string state = filepath.substr(0, 9);

		// Nếu đã hoàn thành
		if (state == "Completed")
		{
			// Lấy đường dẫn đến file
			std::string filename = filepath.substr(14, filepath.length());
			for (int i = 0; i < m_download_info.size(); ++i)
			{
				if (filename == m_download_info[i].filename)
				{
					std::string filepath = m_download_info[i].filepath;
					ShellExecuteA(NULL, NULL, filepath.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}
		else
		{
			AfxMessageBox(_T("File is being downloaded!"), MB_ICONSTOP | MB_OK, 0);
		}
	}
}

