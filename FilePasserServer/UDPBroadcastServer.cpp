#include "pch.h"
#include "UDPBroadcastServer.h"

UDPBroadcastServer::UDPBroadcastServer(CListBox& logMessage, CProgressCtrl& fileProgress)
	: SocketServer(logMessage, fileProgress), m_logMessage(logMessage), m_fileProgress(fileProgress)
{

}

void UDPBroadcastServer::createSocket()
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
}

void UDPBroadcastServer::bindSocket()
{
	if (::bind(m_socket, (sockaddr*)&addr_server, sizeof(addr_server)) != 0)
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
}

void UDPBroadcastServer::recvFile()
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

	while (INFINITE_LOOP)
	{
		int bytes = 0;
		int totalBytes = 0;
		int fileNameSize = 0, pos = 0, fileSize = 0;

		int len = sizeof(addr_server);
		char* buf = new char[DGRM_SIZE];
		char* fileSizeBuf = new char[4];
		char* path = new char[PATH_SIZE];
		char nameBuf[FILE_NAME_SIZE];

		m_fileProgress.SetPos(pos);

		strcpy_s(path, PATH_SIZE, "./");
		memset(buf, 0, DGRM_SIZE);
		memset(fileSizeBuf, 0, 4);
		
		m_logMessage.AddString(strTime);
		m_logMessage.AddString(L"Waiting client UDPSocket for download file");

		fileNameSize = recvfrom(m_socket, nameBuf, FILE_NAME_SIZE, 0, (SOCKADDR*)&addr_server, &len);
		if (fileNameSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				  m_eStr.Format(_T("File read error code: %d"), GetLastError());
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

		fileSize = recvfrom(m_socket, fileSizeBuf, sizeof(int), 0, (SOCKADDR*)&addr_server, &len);
		if (fileSize == SOCKET_ERROR)
		{
			#ifdef DEBUG
				  m_eStr.Format(_T("File read error code: %d"), GetLastError());
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
		
		m_logMessage.AddString(strTime);
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

		while ((bytes = recvfrom(m_socket, buf, DGRM_SIZE, 0, (SOCKADDR*)&addr_server, &len)) != 0)
		{
			//m_logMessage.AddString(L"Downloading file...");
			if (bytes == SOCKET_ERROR)
			{
				#ifdef DEBUG
					m_eStr.Format(_T("File read error code: %d"), GetLastError());
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
		
		m_logMessage.AddString(strTime);
		m_logMessage.AddString(L"Download complete");
	}
}

void UDPBroadcastServer::BroadcastStart()
{
	createSocket();
	bindSocket();
	recvFile();
}

UDPBroadcastServer::~UDPBroadcastServer()
{
	if (m_recvfp != nullptr)
	{
		fclose(m_recvfp);
	}

	closesocket(m_socket);
}