// SocketServer.cpp : implementation file
#pragma comment(lib, "Ws2_32.lib")
#include "pch.h"
#include "FilePasserServerDlg.h"
#include "SocketServer.h"
#include <string>
#include <filesystem>
#include <chrono>

/*#define netconfig->addr_ip "192.168.012.272"
#define ADDR_GROUP "235.0.0.27"
#define netconfig->port 20200*/
#define MAX_CONNECTION 5
#define BUF_SIZE 1000000
#define SIZE 5000
#define FILE_NAME_SIZE 50
#define PATH_SIZE 100

typedef struct {
	char addr_ip[16];
	char addr_group_ip[16];
	int  port;
} network_config;

network_config* netconfig = (network_config*)malloc(sizeof(network_config));

// SocketServer
SocketServer::SocketServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime) 
	: m_socket(NULL), m_recvfp(nullptr), m_error(0), m_accept(NULL)
	, m_logMessage(logMessage), m_fileProgress(fileProgress), m_strTime(strTime)
{
	// Winsock 초기화 
	WSADATA wsa;
	int m_error = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (m_error != 0)
	{
		m_error = WSAGetLastError();
		cout << "WSA initialization Error Code: " << m_error << endl;
		return;
	}

	char port[7];
	FILE* config_fp = NULL;
	errno_t err_f = fopen_s(&config_fp, "network_config.txt", "r");
	if (err_f != 0)
	{
		return;
	}
	fgets(netconfig->addr_ip, sizeof(char) * 16, config_fp);
	netconfig->addr_ip[strlen(netconfig->addr_ip) - 1] = '\0';
	fgets(netconfig->addr_group_ip, sizeof(char) * 16, config_fp);
	netconfig->addr_group_ip[strlen(netconfig->addr_group_ip) - 1] = '\0';
	fgets(port, sizeof(char) * 16, config_fp);

	netconfig->port = atoi(port);
	fclose(config_fp);
}

//////////////////////////////////////////
// TCP 서버 구동을 위한 메소드
void SocketServer::TCPServerStart()
{
	// TCP소켓 생성 및 소켓 예외처리
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		#ifdef DEBUG
			m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		#else
			AfxMessageBox(_T("Create Socket Error"));
		#endif
			return;
	}

	// 소켓 주소 구조체 선언
	struct sockaddr_in server, client;				
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(netconfig->addr_ip);
	server.sin_port = htons(netconfig->port);

	// 소켓 바인딩
	if (::bind(m_socket, (sockaddr*)&server, sizeof(server)) != 0)	
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		#else
			AfxMessageBox(_T("Bind failed Error"));
		#endif
			return;
	}

	// 소켓 리슨
	if (listen(m_socket, SOMAXCONN_HINT(MAX_CONNECTION) != 0))
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Listen failed Error Code: %d"), m_error);
		#else
			AfxMessageBox(_T("Listen failed Error"));
		#endif
			return;
	}

	int clientLen = sizeof(struct sockaddr_in);

	// 소켓 Accept
	m_logMessage.AddString(m_strTime);
	m_logMessage.AddString(L"Waiting accept to client socket...");
	m_fileProgress.SetRange(0, 100);

	m_accept = accept(m_socket, (sockaddr*)&client, &clientLen);
	if (m_accept == INVALID_SOCKET)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Accept failed Error Code: %d"), m_error);
		#else
			AfxMessageBox(L"Accept close");
		#endif
		return;
	}
	m_logMessage.AddString(m_strTime);
	m_logMessage.AddString(L"Successful client connecting");

	/* 파일을 담을 바이트 크기와 버퍼 크기 */
	while (INFINITE_LOOP)
	{
		int bytes = 0, totalBytes = 0, bufferNum = 0, totalBufferNum = 0;
		int fileNameSize = 0, file_size = 0, pos = 0;

		char* buf = new char[BUF_SIZE];
		char* path = new char[PATH_SIZE];
		char nameBuf[FILE_NAME_SIZE];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, BUF_SIZE);
		memset(nameBuf, 0, FILE_NAME_SIZE);
		fileNameSize = recv(m_accept, nameBuf, FILE_NAME_SIZE, 0);
		if (fileNameSize == 0)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File open error Code: %d") , GetLastError());
			#else	
				AfxMessageBox(_T("Client disconnect"));
			#endif
				return;
		}
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		file_size = atol(buf);
		totalBufferNum = (file_size / BUF_SIZE) + 1;
		bufferNum = 0;

		// 해당 위치에 파일 개방
		errno_t ferr = fopen_s(&m_recvfp, path, "wb");
		if (ferr != 0)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File open error Code: %d"), ferr);
			#else	
				AfxMessageBox(_T("Client disconnect"));
			#endif
				return;
		}

		WCHAR path_t[PATH_SIZE];
		size_t* size = new size_t;
		mbstowcs_s(size, path_t, PATH_SIZE, path, PATH_SIZE);

		while (bufferNum != totalBufferNum)
		{
			if ((bytes = recv(m_accept, buf, BUF_SIZE, 0)) != 0)
			{
				m_logMessage.AddString(m_strTime);
				m_logMessage.AddString(L"Downloading file....");
				m_logMessage.AddString(path_t);
				bufferNum++;
				fwrite(buf, sizeof(char), bytes, m_recvfp);
			}

			else if (bytes == SOCKET_ERROR)
			{
				#ifdef DEBUG
					m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				#else
					AfxMessageBox(_T("Client disconnect"));
				#endif	
					return;
			}
		}
		// 파일 저장 버퍼와 경로 할당 해제
		m_fileProgress.SetPos(100);
		if ((AfxMessageBox(_T("Download complete"), MB_OK) == IDOK)) 
		{
			m_fileProgress.SetPos(0);
		}
		else
		{
		}
			
		fclose(m_recvfp);
		delete[] path;
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Download complete");
	}
}


//////////////////////////////////////////
// UDP Unicast 서버 구동을 위한 메소드
void SocketServer::UDPServerStart()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// 소켓 예외처리
	if (m_socket == INVALID_SOCKET)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		#else
			AfxMessageBox(_T("Create socket failed"));
		#endif
			return;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(netconfig->addr_ip);
	server.sin_port = htons(netconfig->port);

	if (::bind(m_socket, (const sockaddr*)&server, sizeof(server)))
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		#else
			AfxMessageBox(_T("Bind failed Error"));
		#endif
			return;
	}

	while (INFINITE_LOOP)
	{
		int bytes = 0;
		int totalBytes = 0;
		int fileNameSize = 0, pos = 0, fileSize = 0;

		int len = sizeof(server);
		char* buf = new char[SIZE];
		char* fileSizeBuf = new char[4];
		char* path = new char[PATH_SIZE];
		char nameBuf[FILE_NAME_SIZE];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, SIZE);
		memset(fileSizeBuf, 0, 4);
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Waiting client UDPSocket for download file.");

		fileNameSize = recvfrom(m_socket, nameBuf, FILE_NAME_SIZE, 0, (SOCKADDR*)&server, &len);
		if (fileNameSize == INVALID_SOCKET)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(_T("Close UDPSocket"));
			#endif
				return;
		}
		else{ }

		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		fileSize = recvfrom(m_socket, fileSizeBuf, 4, 0, (SOCKADDR*)&server, &len);
		if (fileSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(L"Disconnected with Client");
			#endif
				return;
		}
		else if (m_socket == INVALID_SOCKET)
		{
			AfxMessageBox(L"Disconnected with Client");
			return;
		}
		memcpy_s(&totalBytes, 4, fileSizeBuf, 4);
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Downloading file...");
		

		m_fileProgress.SetRange(0, totalBytes);

		errno_t ferr = fopen_s(&m_recvfp, path, "wb");
		if (ferr != 0)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File open error Code: %d"), ferr);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(_T("File open error"));
			#endif
				return;
		}

		WCHAR path_t[PATH_SIZE];
		size_t* size = new size_t;
		mbstowcs_s(size, path_t, PATH_SIZE, path, PATH_SIZE);
		m_logMessage.AddString(path_t);

		while ((bytes = recvfrom(m_socket, buf, SIZE, 0, (SOCKADDR*)&server, &len)) != 0)
		{
			//m_logMessage.AddString(L"Downloading file...");
			if (bytes == SOCKET_ERROR)
			{
				#ifdef DEBUG
					m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
					AfxMessageBox(m_eStr);
				#else
					AfxMessageBox(_T("Close UDPSocket"));
				#endif
					return;
			}
			else if (m_socket == INVALID_SOCKET)
			{
				AfxMessageBox(L"Disconnected with Client");
				return;
			}
			fwrite(buf, sizeof(char), bytes, m_recvfp);
			totalBytes -= bytes;
			pos += bytes;
			if (totalBytes < 0)
				break;
		}

		m_fileProgress.SetPos(pos);
		if ((AfxMessageBox(_T("Download complete"), MB_OK | MB_ICONINFORMATION) == IDOK))
			m_fileProgress.SetPos(0);

		fclose(m_recvfp);
		delete[] path;
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Download complete");
	}
}


//////////////////////////////////////////
// UDP Broadcast 서버 구동을 위한 메소드
void SocketServer::UDPBroadServerStart()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
			AfxMessageBox(m_eStr);
		#else
			AfxMessageBox(_T("Create socket failed"));
		#endif
			return;
	}

	char broadcast = '1';

	if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) != 0)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Setsockopt failed Error Code: %d"), m_error);
			AfxMessageBox(m_eStr);
		#else
			AfxMessageBox(_T("Setsockopt failed Error"));
		#endif
			return;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(netconfig->addr_ip);
	server.sin_port = htons(netconfig->port);

	if (::bind(m_socket, (sockaddr*)&server, sizeof(server)) != 0)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
			AfxMessageBox(m_eStr);
		#else
			AfxMessageBox(_T("Bind failed error"));
		#endif	
			return;
	}

	while (INFINITE_LOOP)
	{
		int bytes = 0;
		int totalBytes = 0;
		int fileNameSize = 0, pos = 0, fileSize = 0;

		int len = sizeof(server);
		char* buf = new char[SIZE];
		char* fileSizeBuf = new char[4];
		char* path = new char[PATH_SIZE];
		char nameBuf[FILE_NAME_SIZE];

		m_fileProgress.SetPos(pos);

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, SIZE);
		memset(fileSizeBuf, 0, 4);
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Waiting client UDPSocket for download file");

		fileNameSize = recvfrom(m_socket, nameBuf, FILE_NAME_SIZE, 0, (SOCKADDR*)&server, &len);
		if (fileNameSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(L"Disconnected with Client");
			#endif	
				return;
		}
		else if (m_socket == INVALID_SOCKET)
		{
			AfxMessageBox(L"Disconnected with Client");
			return;
		}
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);
		
		fileSize = recvfrom(m_socket, fileSizeBuf, sizeof(int), 0, (SOCKADDR*)&server, &len);
		if (fileSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
			#else	
				AfxMessageBox(L"Disconnected with Client");
			#endif
				return;
		}
		else if (m_socket == INVALID_SOCKET)
		{
			AfxMessageBox(L"Disconnected with Client");
			return;
		}
		memcpy_s(&totalBytes, sizeof(int), fileSizeBuf, sizeof(int));
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Downloading file...");

		m_fileProgress.SetRange(0, totalBytes);

		errno_t ferr = fopen_s(&m_recvfp, path, "wb");
		if (ferr != 0)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File open error Code: %d"), ferr);
				AfxMessageBox(m_eStr);
			#else	
				AfxMessageBox(_T("File open error"));
			#endif
				return;
		}

		WCHAR path_t[PATH_SIZE];
		size_t* size = new size_t;
		mbstowcs_s(size, path_t, PATH_SIZE, path, PATH_SIZE);
		m_logMessage.AddString(path_t);

		while ((bytes = recvfrom(m_socket, buf, SIZE, 0, (SOCKADDR*)&server, &len)) != 0)
		{
			//m_logMessage.AddString(L"Downloading file...");
			if (bytes == SOCKET_ERROR)
			{
				#ifdef DEBUG
					m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
					AfxMessageBox(m_eStr);
				#else	
					AfxMessageBox(_T("File read error"));
				#endif
					return;
			}
			else if (m_socket == INVALID_SOCKET)
			{
				AfxMessageBox(L"Disconnected with Client");
				return;
			}
			totalBytes -= bytes;
			pos += bytes;
			fwrite(buf, sizeof(char), bytes, m_recvfp);
			if (totalBytes < 0)
				break;
		}
		m_fileProgress.SetPos(pos);
		if ((AfxMessageBox(_T("Download complete"), MB_OK | MB_ICONINFORMATION) == IDOK))
			m_fileProgress.SetPos(0);

		fclose(m_recvfp);
		delete[] path;
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Download complete");
	}
}


//////////////////////////////////////////
// UDP Multicast 서버 구동을 위한 메소드
void SocketServer::UDPMultiServerStart()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_socket == INVALID_SOCKET)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
			AfxMessageBox(m_eStr);
		#else
			AfxMessageBox(_T("Create socket failed"));
		#endif
			return;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(netconfig->addr_ip);
	server.sin_port = htons(netconfig->port);

	if (::bind(m_socket, (const sockaddr*)&server, sizeof(server)))
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
			AfxMessageBox(m_eStr);
		#else
			AfxMessageBox(_T("Bind failed Error"));
		#endif
			return;
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.S_un.S_addr = inet_addr(netconfig->addr_group_ip);
	mreq.imr_interface.S_un.S_addr = htonl(INADDR_ANY);

	if (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0)
	{
		#ifdef DEBUG
			m_error = WSAGetLastError();
			m_eStr.Format(_T("Setsockopt failed Error Code: %d"), m_error);
			AfxMessageBox(m_eStr);
		#else
			AfxMessageBox(_T("Setsockopt failed"));
		#endif
			return;
	}

	while (INFINITE_LOOP)
	{
		int bytes = 0;
		int totalBytes = 0;
		int fileNameSize = 0, pos = 0, fileSize = 0;

		int len = sizeof(server);
		char* buf = new char[SIZE];
		char* fileSizeBuf = new char[4];
		char* path = new char[PATH_SIZE];
		char nameBuf[FILE_NAME_SIZE];

		m_fileProgress.SetPos(pos);

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, SIZE);
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Waiting client UDPSocket for download file");

		fileNameSize = recvfrom(m_socket, nameBuf, FILE_NAME_SIZE, 0, (SOCKADDR*)&server, &len);
		if (fileNameSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(L"Disconnected with Client");
			#endif
				return;
		}
		else if (m_socket == INVALID_SOCKET)
		{
			AfxMessageBox(L"Disconnected with Client");
			return;
		}
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		fileSize = recvfrom(m_socket, fileSizeBuf, sizeof(int), 0, (SOCKADDR*)&server, &len);
		if (fileSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(_T("File read error"));
			#endif	
				return;
		}
		else if (m_socket == INVALID_SOCKET)
		{
			AfxMessageBox(L"Disconnected with Client");
			return;
		}
		memcpy_s(&totalBytes, sizeof(int), fileSizeBuf, sizeof(int));
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Downloading file...");

		m_fileProgress.SetRange(0, totalBytes);

		errno_t ferr = fopen_s(&m_recvfp, path, "wb");
		if (ferr != 0)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("File open error Code: %d"), ferr);
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(_T("File open error"));
			#endif
				return;
		}

		WCHAR path_t[PATH_SIZE];
		size_t* size = new size_t;
		mbstowcs_s(size, path_t, PATH_SIZE, path, PATH_SIZE);
		m_logMessage.AddString(path_t);

		while ((bytes = recvfrom(m_socket, buf, SIZE, 0, (SOCKADDR*)&server, &len)) != 0)
		{
			//m_logMessage.AddString(L"Downloading file...");
			if (bytes == SOCKET_ERROR)
			{
				#ifdef DEBUG
					m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
					AfxMessageBox(m_eStr);
				#else	
					AfxMessageBox(L"Disconnected with Client");
				#endif
					return;
				}
			else if (m_socket == INVALID_SOCKET)
			{
				AfxMessageBox(L"Disconnected with Client");
				return;
			}
			totalBytes -= bytes;
			pos += bytes;
			fwrite(buf, sizeof(char), bytes, m_recvfp);
			if (totalBytes < 0)
				break;
		}
		m_fileProgress.SetPos(pos);
		if ((AfxMessageBox(_T("Download complete"), MB_OK | MB_ICONINFORMATION) == IDOK))
			m_fileProgress.SetPos(0);
		fclose(m_recvfp);
		delete[] path;
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Downloading complete");
	}
}

SocketServer::~SocketServer()
{
	if (m_recvfp != nullptr)
		fclose(m_recvfp);
	closesocket(m_socket);
	if (m_accept != NULL)
		closesocket(m_accept);
	WSACleanup();
}
// SocketServer member functions