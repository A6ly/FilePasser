#include "pch.h"
#include "TCPServer.h"
#include <process.h>

struct sockaddr_in addr_server, addr_client;

TCPServer::TCPServer(CListBox& logMessage, CProgressCtrl& fileProgress) 
	: SocketServer(logMessage, fileProgress), m_newSocket(0)
	, m_logMessage(logMessage), m_fileProgress(fileProgress)
{

}

void TCPServer::createSocket()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		#ifdef DEBUG
				m_eStr.Format(_T("Create socket failed Error Code: %d"), WSAGetLastError());
				AfxMessageBox(m_eStr);
		#else
				AfxMessageBox(_T("Create Socket Error"));
		#endif
				return;
	}
}

void TCPServer::bindSocket()
{
	if (::bind(m_socket, (sockaddr*)&addr_server, sizeof(addr_server)) != 0)
	{
		#ifdef DEBUG
				m_eStr.Format(_T("Bind failed Error Code: %d"), WSAGetLastError());
				AfxMessageBox(m_eStr);
		#else
				AfxMessageBox(_T("Bind failed Error"));
		#endif
				return;
	}
}

void TCPServer::acceptSocket()
{
	if (listen(m_socket, SOMAXCONN_HINT(8) != 0))
	{
		#ifdef DEBUG
				m_eStr.Format(_T("Listen failed Error Code: %d"), WSAGetLastError());
				AfxMessageBox(m_eStr);
		#else
				AfxMessageBox(_T("Listen failed Error"));
		#endif
				return;
	}

	int clientLen = sizeof(struct sockaddr_in);

	time_t t = time(NULL);
	struct tm tm;
	errno_t err_t = localtime_s(&tm, &t);
	if (err_t != 0)
	{
		m_logMessage.AddString(L"Time ERROR!");
	}
	CString strTime;
	strTime.Format(L"%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	// 소켓 Accept
	
	m_logMessage.AddString(strTime);
	m_logMessage.AddString(L"Waiting accept to client socket...");
	
	m_fileProgress.SetRange(0, 100);
	
	m_newSocket = accept(m_socket, (sockaddr*)&addr_client, &clientLen);
	if (m_newSocket == INVALID_SOCKET)
	{
	#ifdef DEBUG
			m_eStr.Format(_T("Accept failed Error Code: %d"), WSAGetLastError());
			AfxMessageBox(m_eStr);
	#else
			AfxMessageBox(L"Accept close");
	#endif
			return;
	}

	
	m_logMessage.AddString(strTime);
	m_logMessage.AddString(L"Successful client connecting");
	
}

void TCPServer::recvFile(SOCKET accept)
{
	time_t t = time(NULL);
	struct tm tm;
	errno_t err_t = localtime_s(&tm, &t);
	if (err_t != 0)
	{
		m_logMessage.AddString(L"Time ERROR!");
	}
	CString strTime;
	strTime.Format(L"%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	while (true)
	{
		int bytes = 0, totalBytes = 0, bufferNum = 0, totalBufferNum = 0;
		int fileNameSize = 0, file_size = 0, pos = 0;

		char* buf = new char[BUF_SIZE];
		char* path = new char[PATH_SIZE];
		char nameBuf[FILE_NAME_SIZE];

		strcpy_s(path, PATH_SIZE, "./");
		//strcpy_s(path, PATH_SIZE, "C:/Users/Park/Desktop/recv/");

		memset(buf, 0, BUF_SIZE);
		memset(nameBuf, 0, FILE_NAME_SIZE);
		fileNameSize = recv(accept, nameBuf, FILE_NAME_SIZE, 0);
		if (fileNameSize == 0)
		{
			#ifdef DEBUG
				  m_eStr.Format(_T("File open error Code: %d"), GetLastError());
				  AfxMessageBox(m_eStr);
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
			if ((bytes = recv(accept, buf, BUF_SIZE, 0)) != 0)
			{
				
				m_logMessage.AddString(strTime);
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
		delete[] buf;
		delete[] path;
		
		m_logMessage.AddString(strTime);
		m_logMessage.AddString(L"Download complete");
	}
}

void TCPServer::TCPServerStart()
{
	createSocket();
	bindSocket();
	acceptSocket();
	recvFile(m_newSocket);
}

TCPServer::~TCPServer()
{
	if (m_recvfp != nullptr)
	{
		fclose(m_recvfp);
	}

	if (m_newSocket != 0)
	{
		closesocket(m_newSocket);
	}
	closesocket(m_socket);
}
