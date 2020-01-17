
// FilePasserServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FilePasserServer.h"
#include "FilePasserServerDlg.h"
#include "afxdialogex.h"
#include "SocketServer.h"

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


// CFilePasserServerDlg dialog



CFilePasserServerDlg::CFilePasserServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEPASSERSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilePasserServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFilePasserServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTBUTTON, &CFilePasserServerDlg::OnBnClickedStartbutton)
	ON_WM_GETMINMAXINFO()
	ON_LBN_SELCHANGE(IDC_SERVER__LOGLIST, &CFilePasserServerDlg::OnLbnSelchangeServer)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DOWNLOAD__PROGRESS, &CFilePasserServerDlg::OnNMCustomdrawDownload)
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
	lpMMI->ptMinTrackSize.x = 650;
	lpMMI->ptMinTrackSize.y = 500;

	lpMMI->ptMaxTrackSize.x = 650;
	lpMMI->ptMaxTrackSize.y = 500;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

// 버튼 클릭 시 소켓 open
void CFilePasserServerDlg::OnBnClickedStartbutton()
{
	SocketServer* socketServer = new SocketServer();
	if (socketServer->TCPServerStart())
	{
		cout << "go" << endl;
	}
}

// 클라이언트-서버 로그 출력 List
void CFilePasserServerDlg::OnLbnSelchangeServer()
{

}
 
// 수신 파일 전송률 
void CFilePasserServerDlg::OnNMCustomdrawDownload(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
