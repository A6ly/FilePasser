#include "pch.h"
#include "UDPUnicastServer.h"


UDPUnicastServer::UDPUnicastServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime)
	: SocketServer(logMessage, fileProgress, strTime), m_logMessage(logMessage), m_fileProgress(fileProgress)
	, m_strTime(strTime)
{

}

void UDPUnicastServer::createSocket()
{
	m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// 家南 抗寇贸府
	if (m_udpSocket == INVALID_SOCKET)
	{
		#ifdef DEBUG
				m_error = WSAGetLastError();
				m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		#else
				AfxMessageBox(_T("Create socket failed"));
		#endif
				return;
	}
}

void UDPUnicastServer::bindSocket()
{
	if (::bind(m_udpSocket, (const sockaddr*)&addr_server, sizeof(addr_server)))
	{
		#ifdef DEBUG
				m_error = WSAGetLastError();
				m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		#else
				AfxMessageBox(_T("Bind failed Error"));
		#endif
				return;
	}
}

void UDPUnicastServer::recvFile()
{
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

		//strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		strcpy_s(path, PATH_SIZE, "C:/Users/Park/Desktop/recv/");
		memset(buf, 0, DGRM_SIZE);
		memset(fileSizeBuf, 0, 4);
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Waiting client UDPSocket for download file.");

		fileNameSize = recvfrom(m_udpSocket, nameBuf, FILE_NAME_SIZE, 0, (SOCKADDR*)&addr_server, &len);
		if (fileNameSize == INVALID_SOCKET)
		{
		#ifdef DEBUG
			  m_eStr.Format(_T("File read error code: %d"), GetLastError());
			  AfxMessageBox(m_eStr);
		#else
			  AfxMessageBox(_T("Close UDPSocket"));
		#endif
			  delete[] path;
			  delete[] buf;
			  delete[] fileSizeBuf;
			  return;
		}
		else {}

		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		fileSize = recvfrom(m_udpSocket, fileSizeBuf, 4, 0, (SOCKADDR*)&addr_server, &len);
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
		else if (m_udpSocket == INVALID_SOCKET)
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

		while ((bytes = recvfrom(m_udpSocket, buf, DGRM_SIZE, 0, (SOCKADDR*)&addr_server, &len)) != 0)
		{
			//m_logMessage.AddString(L"Downloading file...");
			if (bytes == SOCKET_ERROR)
			{
				#ifdef DEBUG
					  m_eStr.Format(_T("File read error code: %d"), GetLastError());
					  AfxMessageBox(m_eStr);
				#else
					  AfxMessageBox(_T("Close UDPSocket"));
				#endif
					  return;
			}
			else if (m_udpSocket == INVALID_SOCKET)
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
		delete[] buf;
		delete[] fileSizeBuf;
		m_logMessage.AddString(m_strTime);
		m_logMessage.AddString(L"Download complete");
	}
}

void UDPUnicastServer::UnicastStart()
{
	createSocket();
	bindSocket();
	recvFile();
}

UDPUnicastServer::~UDPUnicastServer()
{
	if (m_recvfp != nullptr)
	{
		fclose(m_recvfp);
	}

	closesocket(m_udpSocket);
}