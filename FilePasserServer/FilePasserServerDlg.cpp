
// FilePasserServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FilePasserServer.h"
#include "FilePasserServerDlg.h"
#include "afxdialogex.h"
#include "SocketServer.h"
#include <afxwin.h>
#include <Windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum NetType { TYPE_NONE = 0, TYPE_TCP = 1, TYPE_UDP_UNI = 2, TYPE_UDP_BROAD = 3, TYPE_UDP_MULTI = 4 };

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


// CFilePasserServerDlg dialog

CFilePasserServerDlg::CFilePasserServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEPASSERSERVER_DIALOG, pParent), m_socketServer(new SocketServer(logMessage, fileProgress)), m_netType(0)
	, m_combo_protocol(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilePasserServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER__LOGLIST, logMessage);
	DDX_Control(pDX, IDC_DOWNLOAD__PROGRESS, fileProgress);
}

BEGIN_MESSAGE_MAP(CFilePasserServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
//	ON_LBN_SELCHANGE(IDC_SERVER__LOGLIST, &CFilePasserServerDlg::OnLbnSelchangeServer)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DOWNLOAD__PROGRESS, &CFilePasserServerDlg::OnNMCustomdrawDownload)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CFilePasserServerDlg::OnBnClickedConnectButton)
	ON_BN_CLICKED(IDC_RADIO_SOCKET, &CFilePasserServerDlg::OnBnClickedRadioSocket)
	ON_BN_CLICKED(IDC_RADIO_RS232, &CFilePasserServerDlg::OnBnClickedRadioRs232)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOL, &CFilePasserServerDlg::OnCbnSelchangeComboProtocol)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPROT, &CFilePasserServerDlg::OnCbnSelchangeComboComprot)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CFilePasserServerDlg::OnCbnSelchangeComboBaudrate)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CFilePasserServerDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CFilePasserServerDlg::OnBnClickedButtonClose)
	ON_LBN_SELCANCEL(IDC_SERVER__LOGLIST, &CFilePasserServerDlg::OnSelcancelServerLoglist)
END_MESSAGE_MAP()


// CFilePasserServerDlg message handlers

BOOL CFilePasserServerDlg::OnInitDialog()
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
	//GetDlgItem(IDC_CONNECT_BUTTON)->ShowWindow(TRUE);

	GetDlgItem(IDC_COMBO_COMPROT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(FALSE);

	/*LoadListBox();
	fileProgress.SetRange(0, 100);
	fileProgress.SetPos(0);*/

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFilePasserServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFilePasserServerDlg::OnPaint()
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
HCURSOR CFilePasserServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFilePasserServerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// WindowSize 크기 설정
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 600;

	lpMMI->ptMaxTrackSize.x = 800;
	lpMMI->ptMaxTrackSize.y = 600;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

std::thread threadObj;

// Start 버튼 클릭 시 소켓 open
void CFilePasserServerDlg::OnBnClickedConnectButton()
{
	if (NetType::TYPE_TCP == m_netType)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->TCPServerStart(); });
		logMessage.AddString(L"TCP Server Open");

		m_netType = -1;
	}
	else if (NetType::TYPE_UDP_UNI == m_netType)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->UDPServerStart(); });
		logMessage.AddString(L"UDP Unicast Server Open");

		m_netType = -1;
	}
	else if (NetType::TYPE_UDP_BROAD == m_netType)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->UDPBroadServerStart(); });
		logMessage.AddString(L"UDP Broadcast Server Open");

		m_netType = -1;
	}
	else if (NetType::TYPE_UDP_MULTI == m_netType)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->UDPMultiServerStart(); });
		logMessage.AddString(L"UDP Multicast Server Open");

		m_netType = -1;
	}
	else if (NetType::TYPE_NONE == m_netType)
	{
		AfxMessageBox(L"Please select NetType to open server");
	}
	else
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Start");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(true);

		m_socketServer->~SocketServer();
		if (threadObj.joinable())
			threadObj.join();
		m_socketServer = new SocketServer(logMessage, fileProgress);
		m_combo_protocol->SetCurSel(0);
		m_netType = TYPE_NONE;
		logMessage.AddString(L"Server Close Connection");
	}
}

void CFilePasserServerDlg::LoadListBox()
{

}
 
// 수신 파일 전송률 
void CFilePasserServerDlg::OnNMCustomdrawDownload(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CFilePasserServerDlg::OnBnClickedRadioSocket()
{
	GetDlgItem(IDC_COMBO_COMPROT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);

	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(TRUE);
}

void CFilePasserServerDlg::OnBnClickedRadioRs232()
{
	GetDlgItem(IDC_COMBO_COMPROT)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(TRUE);

	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(FALSE);
}

void CFilePasserServerDlg::OnCbnSelchangeComboProtocol()
{
	m_combo_protocol = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PROTOCOL));
	m_netType = static_cast<short>(m_combo_protocol->GetCurSel());
}

void CFilePasserServerDlg::OnCbnSelchangeComboComprot()
{
	// TODO: Add your control notification handler code here
}

void CFilePasserServerDlg::OnCbnSelchangeComboBaudrate()
{
	// TODO: Add your control notification handler code here
}

void CFilePasserServerDlg::OnBnClickedButtonOpen()
{
	// TODO: Add your control notification handler code here
}

void CFilePasserServerDlg::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
}

void CFilePasserServerDlg::OnSelcancelServerLoglist()
{
	// TODO: Add your control notification handler code here
}
