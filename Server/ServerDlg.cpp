// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
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


// CServerDlg dialog



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SHARE, m_bnShare);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_bnAdd);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_bnDelete);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SHARE, &CServerDlg::OnBnClickedButtonShare)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_HISTORY, &CServerDlg::OnBnClickedButtonClearHistory)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CServerDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CServerDlg::OnBnClickedButtonDelete)
	ON_LBN_SETFOCUS(IDC_LIST_FILES, &CServerDlg::OnLbnSetfocusListFiles)
	ON_LBN_KILLFOCUS(IDC_LIST_FILES, &CServerDlg::OnLbnKillfocusListFiles)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
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

	// Khởi tạo thư viện Socket
	if (AfxSocketInit() == FALSE)
	{
		throw "AfxSocketInit Error!";
	}

	// Gửi list box handler cho server
	m_server.SetListFiles((CListBox*)this->GetDlgItem(IDC_LIST_HISTORY));
	m_server.SetListOnline((CListBox*)this->GetDlgItem(IDC_LIST_ONLINE));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnOK()
{
}

void CServerDlg::OnCancel()
{
	if (m_isShared)
	{
		m_server.Close();
	}
	CDialogEx::OnCancel();
}



void CServerDlg::OnBnClickedButtonShare()
{
	if (m_isShared)
	{
		m_isShared = FALSE;
		m_bnShare.SetWindowTextW(_T("Share"));
		m_server.Close();
	}
	else
	{
		m_isShared = TRUE;
		m_bnShare.SetWindowTextW(_T("Unshare"));
		m_server.Create(PORT, SOCK_STREAM, NULL);
		m_server.AsyncSelect(FD_ACCEPT | FD_READ | FD_WRITE);
		m_server.Listen(5);
	}
}

void CServerDlg::OnBnClickedButtonClearHistory()
{
	CListBox* list_history = (CListBox*)this->GetDlgItem(IDC_LIST_HISTORY);
	list_history->ResetContent();
	hAdjustScroll(list_history);
}


void CServerDlg::OnBnClickedButtonAdd()
{
	int bufferMaxLength = 10 * (260 + 1) + 1;

	// Tạo trước buffer để địa chỉ được chọn lưu vào
	// Độ dài tối thiểu của buffer trong sFileName là bufferMaxLength
	CString sFileName;
	wchar_t* p = sFileName.GetBuffer(bufferMaxLength);

	// Tạo cửa sổ chọn file và cho phép chọn nhiều file
	CFileDialog browseDlg(TRUE);
	OPENFILENAME& ofn = browseDlg.GetOFN();
	ofn.Flags |= OFN_ALLOWMULTISELECT;
	ofn.lpstrFilter = _T("All Files(*.*) | *.* || ");

	// Chỉ địng địa chỉ trả về lưu vào buffer đã chọn
	// Dạng lưu: "duongdan\0duongdan\0duongdan\0\0\0\0..."
	// Độ dài tối đa địa chỉ trả về: bufferMaxLength
	ofn.lpstrFile = p;
	ofn.nMaxFile = bufferMaxLength;

	// Hiển thị cửa sổ chọn file
	if (browseDlg.DoModal() == IDOK)
	{
		// Sau khi thay đổi buffer của sFileName, cần gọi ReleaseBuffer()
		// để sử dụng các hàm khác trong sFileName
		sFileName.ReleaseBuffer();

		wchar_t* pBufEnd = p + bufferMaxLength - 2;
		wchar_t* start = p;

		// Đoạn đầu là đường dẫn thư mục
		CString filepath(start);
		while ((p < pBufEnd) && (*p))
			p++;

		if (p > start)
		{
			p++;
			// Nếu chỉ có 1 file
			if (!((p < pBufEnd) && (*p)))
			{
				hAddFile(filepath);
				return;
			}

			while ((p < pBufEnd) && (*p))
			{
				// Thêm địa chỉ tập tin vào m_fileList
				start = p;
				hAddFile(filepath + _T('\\') + CString(start));

				while ((p < pBufEnd) && (*p))
					p++;
				p++;
			}
		}
	}
}


void CServerDlg::OnBnClickedButtonDelete()
{
	CListBox* listfiles = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);
	UINT uiSelect = listfiles->GetCurSel();
	if (uiSelect == LB_ERR)
		return;

	CString filename;
	int size = listfiles->GetTextLen(uiSelect);
	listfiles->GetText(uiSelect, filename.GetBuffer(size));
	filename.ReleaseBuffer();

	listfiles->DeleteString(uiSelect);
	CT2CA tmp(filename);
	m_server.DeleteSharedFile(std::string(tmp));

	if (listfiles->GetCount() == 0)
	{
		listfiles->EnableWindow(FALSE);
		m_bnDelete.EnableWindow(FALSE);
	}
	else
	{
		listfiles->SetCurSel(min(uiSelect, listfiles->GetCount() - 1));
	}
	hAdjustScroll((CListBox*)this->GetDlgItem(IDC_LIST_FILES));
}

void CServerDlg::OnLbnSetfocusListFiles()
{
	CListBox* listfiles = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);
	if (listfiles->GetCount() > 0)
		m_bnDelete.EnableWindow(TRUE);
	else
		m_bnDelete.EnableWindow(FALSE);
}


void CServerDlg::OnLbnKillfocusListFiles()
{
	CListBox* listfiles = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);
	if (listfiles->GetCount() > 0)
	{
		UINT uiSelect = listfiles->GetCurSel();
		if (uiSelect == LB_ERR)
			m_bnDelete.EnableWindow(FALSE);
		else
			m_bnDelete.EnableWindow(TRUE);
	}
	else
		m_bnDelete.EnableWindow(FALSE);
}

void CServerDlg::hAddFile(CString filepath)
{
	CT2CA tmp(filepath);
	std::string file(tmp);
	std::string filename = file.substr(file.find_last_of('\\') + 1, file.length());

	CListBox* listfiles = (CListBox*)this->GetDlgItem(IDC_LIST_FILES);
	listfiles->EnableWindow(TRUE);
	if (listfiles->GetCount() == 0 || listfiles->FindString(0, CString(filename.c_str())) == LB_ERR)
	{
		listfiles->AddString(CString(filename.c_str()));
		listfiles->SetTopIndex(listfiles->GetCount() - 1);
		hAdjustScroll(listfiles);
		m_server.AddSharedFile(file);
	}
}

void CServerDlg::hAdjustScroll(CListBox * listbox)
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