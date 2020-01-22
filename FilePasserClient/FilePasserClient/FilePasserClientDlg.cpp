
// FilePasserClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "FilePasserClient.h"
#include "FilePasserClientDlg.h"
#include "afxdialogex.h"
#include "SocketClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define PORT 30000
#define BUF_SIZE 1048576
#endif

struct sockaddr_in mcast_group;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.

	enum { IDD = IDD_ABOUTBOX };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CFilePasserClientDlg 대화 상자



CFilePasserClientDlg::CFilePasserClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEPASSERCLIENT_DIALOG, pParent)
{
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilePasserClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	DDX_Control(pDX, IDC_PROTOCOL_COMBO, m_comboProtocolList);
}

BEGIN_MESSAGE_MAP(CFilePasserClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_IP_EDIT, &CFilePasserClientDlg::OnEnChangeEdit1)
	ON_WM_GETMINMAXINFO()
	ON_EN_CHANGE(IDC_PORT_EDIT, &CFilePasserClientDlg::OnEnChangePortEdit)
	ON_BN_CLICKED(IDC_Log, &CFilePasserClientDlg::OnBnClickedLog)
	ON_BN_CLICKED(IDC_BUTTON_FILESEND, &CFilePasserClientDlg::OnBnClickedButtonFilesend)
//	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CFilePasserClientDlg::OnBnClickedButtonConnect)
ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CFilePasserClientDlg::OnBnClickedButtonConnect)
END_MESSAGE_MAP()


// CFilePasserClientDlg 메시지 처리기

BOOL CFilePasserClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_comboProtocolList.AddString(_T("TCP/IP"));
	m_comboProtocolList.AddString(_T("UDP Unicast"));
	m_comboProtocolList.AddString(_T("UDP Multicast"));
	m_comboProtocolList.AddString(_T("UDP broadcast"));


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFilePasserClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFilePasserClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFilePasserClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFilePasserClientDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CFilePasserClientDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 650;
	lpMMI->ptMinTrackSize.y = 500;

	lpMMI->ptMaxTrackSize.x = 650;
	lpMMI->ptMaxTrackSize.y = 500;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CFilePasserClientDlg::OnEnChangePortEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CFilePasserClientDlg::OnBnClickedLog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CFilePasserClientDlg::OnBnClickedButtonConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = m_comboProtocolList.GetCurSel();
	index += 1;
	CString strIp;
	int strPort;
	GetDlgItemText(IDC_IP_EDIT, strIp);
	strPort = GetDlgItemInt(IDC_PORT_EDIT);
	
	if (index == 1) {
		m_SocketClient.SetWnd(this->m_hWnd);   // Sendmessage 활용을 위한 메인의 핸들을 받는 함수
		m_SocketClient.Create();
		if (m_SocketClient.Connect(strIp, strPort) == FALSE) {
			AfxMessageBox(_T("ERROR : Failed to connect Server"), MB_OK | MB_ICONERROR);
			PostQuitMessage(0);   // 프로그램 종료
			return;
		}
		else {
			AfxMessageBox(_T("Successfully connected to Server"), MB_OK | MB_ICONINFORMATION);
		}
	}

	else if (index == 2) {
		m_SocketClient.SetWnd(this->m_hWnd);
		m_SocketClient.Create(AF_INET, SOCK_DGRAM, 0);
		m_SocketClient.SetSockOpt(SO_BROADCAST, NULL, NULL);
		
	}

	else if (index == 3) {
		m_SocketClient.SetWnd(this->m_hWnd);
		m_SocketClient.Create(AF_INET, SOCK_DGRAM, 0);

	}

	else if (index == 4) {
		m_SocketClient.SetWnd(this->m_hWnd);
		m_SocketClient.Create(strPort, SOCK_DGRAM, 0);
	}
}


void CFilePasserClientDlg::OnBnClickedButtonFilesend()
{
	int index = m_comboProtocolList.GetCurSel();
	index += 1;
	CString strIp;
	int strPort;
	GetDlgItemText(IDC_IP_EDIT, strIp);
	strPort = GetDlgItemInt(IDC_PORT_EDIT);

	if (index == 1) {
		int NameLength_send;
		CStringA strFileName_send;
		CString strFilePath_send;
		CFile sendFile_send;

		CFileDialog fd1(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);
		if (fd1.DoModal() == IDOK) {
			strFilePath_send = fd1.GetPathName();
			sendFile_send.Open(strFilePath_send, CFile::modeRead | CFile::typeBinary);

			strFileName_send = sendFile_send.GetFileName();
			NameLength_send = fd1.GetFileName().GetLength();
			char* strName_send = new char[NameLength_send];
			strFileName_send = sendFile_send.GetFileName();
			strName_send = strFileName_send.GetBuffer(NameLength_send);
			m_SocketClient.Send(strName_send, NameLength_send);

			byte* data_send = new byte[BUF_SIZE];

			DWORD dwRead_send;

			do
			{
				dwRead_send = sendFile_send.Read(data_send, BUF_SIZE);
				m_SocketClient.Send(data_send, dwRead_send);

			} while (dwRead_send > 0);

			sendFile_send.Close();
			strFileName_send.ReleaseBuffer(-1);

			strName_send = NULL;
			delete strName_send;
			delete data_send;
		}
	}
	else if (index == 2) {
		int NameLength_send;
		CStringA strFileName_send;
		CString strFilePath_send;
		CFile sendFile_send;

		CFileDialog fd1(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);
		if (fd1.DoModal() == IDOK) {
			strFilePath_send = fd1.GetPathName();
			sendFile_send.Open(strFilePath_send, CFile::modeRead | CFile::typeBinary);

			strFileName_send = sendFile_send.GetFileName();
			NameLength_send = fd1.GetFileName().GetLength();
			char* strName_send = new char[NameLength_send];
			strFileName_send = sendFile_send.GetFileName();
			strName_send = strFileName_send.GetBuffer(NameLength_send);
			m_SocketClient.SendTo(strName_send, NameLength_send, strPort, _T("255.255.255.255"));

			byte* data_send = new byte[BUF_SIZE];

			DWORD dwRead_send;

			do
			{
				dwRead_send = sendFile_send.Read(data_send, BUF_SIZE);
				m_SocketClient.SendTo(data_send, dwRead_send, strPort, _T("255.255.255.255"));

			} while (dwRead_send > 0);

			sendFile_send.Close();
			strFileName_send.ReleaseBuffer(-1);

			strName_send = NULL;
			delete strName_send;
			delete data_send;
		}
	}

	else if (index == 3) {
		int NameLength_send;
		CStringA strFileName_send;
		CString strFilePath_send;
		CFile sendFile_send;

		CFileDialog fd1(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);
		if (fd1.DoModal() == IDOK) {
			strFilePath_send = fd1.GetPathName();
			sendFile_send.Open(strFilePath_send, CFile::modeRead | CFile::typeBinary);

			strFileName_send = sendFile_send.GetFileName();
			NameLength_send = fd1.GetFileName().GetLength();
			char* strName_send = new char[NameLength_send];
			strFileName_send = sendFile_send.GetFileName();
			strName_send = strFileName_send.GetBuffer(NameLength_send);
			m_SocketClient.SendTo(strName_send, NameLength_send, strPort, strIp);

			byte* data_send = new byte[BUF_SIZE];

			DWORD dwRead_send;

			do
			{
				dwRead_send = sendFile_send.Read(data_send, BUF_SIZE);
				m_SocketClient.SendTo(data_send, dwRead_send, strPort, strIp);

			} while (dwRead_send > 0);

			sendFile_send.Close();
			strFileName_send.ReleaseBuffer(-1);

			strName_send = NULL;
			delete strName_send;
			delete data_send;
		}
	}

	else if (index == 4) {
		int NameLength_send;
		CStringA strFileName_send;
		CString strFilePath_send;
		CFile sendFile_send;

		CFileDialog fd1(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);
		if (fd1.DoModal() == IDOK) {
			strFilePath_send = fd1.GetPathName();
			sendFile_send.Open(strFilePath_send, CFile::modeRead | CFile::typeBinary);

			strFileName_send = sendFile_send.GetFileName();
			NameLength_send = fd1.GetFileName().GetLength();
			char* strName_send = new char[NameLength_send];
			strFileName_send = sendFile_send.GetFileName();
			strName_send = strFileName_send.GetBuffer(NameLength_send);
			m_SocketClient.SendTo(strName_send, NameLength_send, strPort, strIp);

			byte* data_send = new byte[BUF_SIZE];

			DWORD dwRead_send;

			do
			{
				dwRead_send = sendFile_send.Read(data_send, BUF_SIZE);
				m_SocketClient.SendTo(data_send, dwRead_send, strPort, strIp);

			} while (dwRead_send > 0);

			sendFile_send.Close();
			strFileName_send.ReleaseBuffer(-1);

			strName_send = NULL;
			delete strName_send;
			delete data_send;
		}
	}
}
