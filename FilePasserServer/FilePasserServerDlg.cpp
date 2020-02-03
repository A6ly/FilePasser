
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
enum BaudrateType {
	BDR_1200 = 0, BDR_2400 = 1, BDR_4800 = 2, BDR_9600 = 3, BDR_14400 = 4,
	BDR_19200 = 5, BDR_38400 = 6, BDR_57600 = 7, BDR_115200 = 8, BDR_128000 = 9, BDR_256000 = 10,
};
enum DatabitType { DATABIT_5 = 0, DATABIT_6 = 1, DATABIT_7 = 2, DATABIT_8 = 3 };
enum StopbitType { STOPBIT_1 = 0, STOPBIT_ONEHALF = 1, STOPBIT_2 = 2 };
enum ParitybitType { PARITYBIT_NO = 0, PARITYBIT_ODD = 1, PARITYBIT_EVEN = 2 };

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
	: CDialogEx(IDD_FILEPASSERSERVER_DIALOG, pParent), m_socketServer(new SocketServer(logMessage, fileProgress))
	, m_serialPort(new SerialPort(logMessage, fileProgress)), m_portName("NaN")
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
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DOWNLOAD__PROGRESS, &CFilePasserServerDlg::OnNMCustomdrawDownload)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CFilePasserServerDlg::OnBnClickedConnectButton)
	ON_BN_CLICKED(IDC_RADIO_SOCKET, &CFilePasserServerDlg::OnBnClickedRadioSocket)
	ON_BN_CLICKED(IDC_RADIO_RS232, &CFilePasserServerDlg::OnBnClickedRadioRs232)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CFilePasserServerDlg::OnBnClickedButtonOpen)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOL, &CFilePasserServerDlg::OnCbnSelchangeComboProtocol)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CFilePasserServerDlg::OnCbnSelchangeComboBaudrate)
	ON_CBN_SELCHANGE(IDC_COMBO_PARITYBIT, &CFilePasserServerDlg::OnCbnSelchangeComboParitybit)
	ON_CBN_SELCHANGE(IDC_COMBO_STOPBIT, &CFilePasserServerDlg::OnCbnSelchangeComboStopbit)
	ON_CBN_SELCHANGE(IDC_COMBO_DATABIT, &CFilePasserServerDlg::OnCbnSelchangeComboDatabit)
	ON_LBN_SELCANCEL(IDC_SERVER__LOGLIST, &CFilePasserServerDlg::OnSelcancelServerLoglist)
	ON_EN_CHANGE(IDC_EDIT_COMPORT, &CFilePasserServerDlg::OnEnChangeEditComport)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CFilePasserServerDlg::OnBnClickedButtonClose)
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

	GetDlgItem(IDC_EDIT_COMPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_DATABIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_STOPBIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_PARITYBIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);

	GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);

	m_netType = TYPE_NONE;
	m_baudrate = -1;
	m_databit = -1;
	m_stopbit = -1;
	m_paritybit = -1;

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
	if (m_netType == NetType::TYPE_TCP)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->TCPServerStart(); });
		logMessage.AddString(L"TCP Server Open");

		m_netType = -1;
	}
	else if (m_netType == NetType::TYPE_UDP_UNI)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->UDPServerStart(); });
		logMessage.AddString(L"UDP Unicast Server Open");

		m_netType = -1;
	}
	else if (m_netType == NetType::TYPE_UDP_BROAD)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->UDPBroadServerStart(); });
		logMessage.AddString(L"UDP Broadcast Server Open");

		m_netType = -1;
	}
	else if (m_netType == NetType::TYPE_UDP_MULTI)
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Stop");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(false);

		threadObj = std::thread([&]() { m_socketServer->UDPMultiServerStart(); });
		logMessage.AddString(L"UDP Multicast Server Open");

		m_netType = -1;
	}
	else if (m_netType == NetType::TYPE_NONE)
	{
		AfxMessageBox(L"Please select Protocol to open server");
	}
	else
	{
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(L"Start");
		GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(true);

		m_socketServer->~SocketServer();
		if (threadObj.joinable())
		{
			threadObj.join();
		}
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
	GetDlgItem(IDC_EDIT_COMPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_DATABIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_STOPBIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_PARITYBIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);

	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(TRUE);
}

void CFilePasserServerDlg::OnBnClickedRadioRs232()
{
	GetDlgItem(IDC_EDIT_COMPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_DATABIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_STOPBIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_PARITYBIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);

	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_PROTOCOL)->EnableWindow(FALSE);
}

void CFilePasserServerDlg::OnCbnSelchangeComboProtocol()
{
	m_combo_protocol = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PROTOCOL));
	m_netType = static_cast<short>(m_combo_protocol->GetCurSel());
}

void CFilePasserServerDlg::OnCbnSelchangeComboBaudrate()
{
	m_combo_baudrate = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_BAUDRATE));
	int baudrate = static_cast<short>(m_combo_baudrate->GetCurSel());

	if (baudrate == BaudrateType::BDR_1200)			{ m_baudrate = 1200; }
	else if (baudrate == BaudrateType::BDR_2400)	{ m_baudrate = 2400; }
	else if (baudrate == BaudrateType::BDR_4800)	{ m_baudrate = 4800; }
	else if (baudrate == BaudrateType::BDR_9600)	{ m_baudrate = 9600; }
	else if (baudrate == BaudrateType::BDR_14400)	{ m_baudrate = 14400; }
	else if (baudrate == BaudrateType::BDR_19200)	{ m_baudrate = 19200; }
	else if (baudrate == BaudrateType::BDR_38400)	{ m_baudrate = 38400; }
	else if (baudrate == BaudrateType::BDR_57600)	{ m_baudrate = 57600; }
	else if (baudrate == BaudrateType::BDR_115200)  { m_baudrate = 115200; }
	else if (baudrate == BaudrateType::BDR_128000)  { m_baudrate = 128000; }
	else if (baudrate == BaudrateType::BDR_256000)  { m_baudrate = 256000; }
	else
	{
		AfxMessageBox(L"Error while selecting baudrate");
		return;
	}
}

void CFilePasserServerDlg::OnCbnSelchangeComboDatabit()
{
	m_combo_databit = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_DATABIT));
	int databit = static_cast<short>(m_combo_databit->GetCurSel());

	if (databit == DatabitType::DATABIT_5)		{ m_databit = 5; }
	else if (databit == DatabitType::DATABIT_6) { m_databit = 6; }
	else if (databit == DatabitType::DATABIT_7) { m_databit = 7; }
	else if (databit == DatabitType::DATABIT_8) { m_databit = 8; }
	else
	{
		AfxMessageBox(L"Error while selecting databit");
		return;
	}	
}

void CFilePasserServerDlg::OnCbnSelchangeComboStopbit()
{
	m_combo_stopbit = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_STOPBIT));
	int stopbit = static_cast<short>(m_combo_stopbit->GetCurSel());

	if (stopbit == StopbitType::STOPBIT_1)				{ m_stopbit = 0; }
	else if (stopbit == StopbitType::STOPBIT_ONEHALF)	{ m_stopbit = 1; }
	else if (stopbit == StopbitType::STOPBIT_2)			{ m_stopbit = 2; }
	else
	{
		AfxMessageBox(L"Error while selecting stopbit");
		return;
	}
}

void CFilePasserServerDlg::OnCbnSelchangeComboParitybit()
{
	m_combo_paritybit = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PARITYBIT));
	int paritybit = static_cast<short>(m_combo_paritybit->GetCurSel());

	if (paritybit == ParitybitType::PARITYBIT_NO)		 { m_paritybit = 0; }
	else if (paritybit == ParitybitType::PARITYBIT_ODD)  { m_paritybit = 1; }
	else if (paritybit == ParitybitType::PARITYBIT_EVEN) { m_paritybit = 2; }
	else
	{
		AfxMessageBox(L"Error while selecting paritybit");
		return;
	}
}

void CFilePasserServerDlg::OnBnClickedButtonOpen()
{
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(TRUE);
	GetDlgItemTextW(IDC_EDIT_COMPORT, m_portName);

	if (m_portName == "NaN")
	{
		AfxMessageBox(L"Please insert Comport Name");
		return;
	}
	else if (m_baudrate == -1)
	{
		AfxMessageBox(L"Please select Baudrate");
		return;
	}
	else if (m_databit == -1) 
	{
		AfxMessageBox(L"Please select Databit");
		return;
	}	
	else if (m_stopbit == -1)
	{
		AfxMessageBox(L"Please select Stopbit");
		return;
	}
	else if (m_paritybit == -1)
	{
		AfxMessageBox(L"Please select Paritybit");
		return;
	}

	if (!m_serialPort->openPort(m_portName))
	{
		AfxMessageBox(L"Error: Can not open serial port");
		return;
	}
	else {}
	if (!m_serialPort->configureSerialSet(m_baudrate, m_databit, m_stopbit, m_paritybit))
	{
		AfxMessageBox(L"Error: Configure Serial Set Error");
		return;
	}
	else 
	{
		logMessage.AddString(L"Serial Port Open!");
	}
	threadObj = std::thread([&]() { m_serialPort->readFile(); });
}

void CFilePasserServerDlg::OnBnClickedButtonClose()
{
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);

	m_serialPort->~SerialPort();
	if (threadObj.joinable())
	{
		threadObj.join();
	}
	logMessage.AddString(L"Serial Port Close!");
}

void CFilePasserServerDlg::OnEnChangeEditComport()
{

}

void CFilePasserServerDlg::OnSelcancelServerLoglist()
{
	
}
