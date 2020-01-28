// SocketServer.cpp : implementation file
#pragma comment(lib, "Ws2_32.lib")
#include "pch.h"
#include "FilePasserServer.h"
#include "SocketServer.h"
#include <string>
#include <filesystem>
#include <chrono>

#define PORT 20200
#define MAX_CONNECTION 5
#define ADDR_IP "192.168.0.27"
#define ADDR_GROUP "235.0.0.27"
#define BUF_SIZE 100000
#define SIZE 1500
#define PATH_SIZE 100

// SocketServer
SocketServer::SocketServer() : m_socket(NULL), m_fp(nullptr), m_error(0), m_accept(NULL)
{
	// Winsock �ʱ�ȭ 
	WSADATA wsa;
	int m_error = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (m_error != 0)
	{
		m_error = WSAGetLastError();
		cout << "WSA initialization Error Code: " << m_error << endl;
		return;
	}
}

/*void SocketServer::initWinsock()
{
	// Winsock �ʱ�ȭ �Լ�
	WSADATA wsa;
	int m_error = WSAStartup(MAKEWORD(2, 2), &wsa);						
	if (m_error != 0)
	{
		m_error = WSAGetLastError();
		cout << "WSA initialization Error Code: " << m_error << endl;
		return;
	}
}*/

// Start ��ư Ŭ�� �� ���� ����
void SocketServer::TCPServerStart()
{
	// TCP���� ���� �� ���� ����ó��
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	// ���� �ּ� ����ü ����
	struct sockaddr_in server, client;				
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	// ���� ���ε�
	if (::bind(m_socket, (sockaddr*)&server, sizeof(server)) != 0)	
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	// ���� ����
	if (listen(m_socket, SOMAXCONN_HINT(8) != 0))					
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Listen failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	int clientLen = sizeof(struct sockaddr_in);

	// ���� Accept
	m_accept = accept(m_socket, (sockaddr*)&client, &clientLen);
	if (m_accept == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Accept failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	/* ������ ���� ����Ʈ ũ��� ���� ũ�� */
	int bytes, totalBytes, bufferNum, totalBufferNum;
	long file_size;
	int fileNameSize = 0;
	while (1)
	{
		char* path = new char[PATH_SIZE];
		char* buf = new char[BUF_SIZE];
		char nameBuf[20];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, BUF_SIZE);
		fileNameSize = recv(m_accept, nameBuf, 20, 0);
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		file_size = atol(buf);
		totalBufferNum = (file_size / BUF_SIZE) + 1;
		bufferNum = 0;
		totalBytes = 0;

		// �ش� ��ġ�� ���� ����
		errno_t ferr = fopen_s(&m_fp, path, "wb");						

		if (ferr != 0)
		{
			m_eStr.Format(_T("File open error Code: %d"), ferr);
			AfxMessageBox(m_eStr);
			return;
		}

		while (bufferNum != totalBufferNum)
		{
			if ((bytes = recv(m_accept, buf, BUF_SIZE, 0)) != 0)
			{
				bufferNum++;
				totalBytes += bytes;
				fwrite(buf, sizeof(char), bytes, m_fp);
			}

			else if (bytes == SOCKET_ERROR)
			{
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
				SocketServer::~SocketServer();
				return;
			}
		}	
		// ���� ���� ���ۿ� ��� �Ҵ� ����
		fclose(m_fp);
		delete[] path;
	}
	//return;
}

void SocketServer::UDPServerStart()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// ���� ����ó��
	if (m_socket == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	struct sockaddr_in server, client;
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(ADDR_IP);
	server.sin_port = htons(PORT);

	if (::bind(m_socket, (const sockaddr*)&server, sizeof(server)))
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	int bytes, totalBytes, bufferNum, totalBufferNum;
	long file_size;
	int fileNameSize = 0;

	while (1)
	{
		int len = sizeof(server);
		char* buf = new char[SIZE];
		char* path = new char[PATH_SIZE];
		char nameBuf[100];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, SIZE);
		fileNameSize = recvfrom(m_socket, nameBuf, 20, 0, (SOCKADDR*)&server, &len);
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		file_size = atol(buf);
		totalBufferNum = file_size / SIZE + 1;
		bufferNum = 0;
		totalBytes = 0;

		errno_t ferr = fopen_s(&m_fp, path, "wb");

		if (ferr != 0)
		{
			m_eStr.Format(_T("File open error Code: %d"), ferr);
			AfxMessageBox(m_eStr);
			return;
		}

		int i = 0;
		while ((bytes = recvfrom(m_socket, buf, SIZE, 0, (SOCKADDR*)&server, &len)) != 0)
		{
			if (bytes == SOCKET_ERROR)
			{
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
				return;
			}

			bufferNum++;
			totalBytes += bufferNum;
			fwrite(buf, sizeof(char), bytes, m_fp);
		}
		fclose(m_fp);
		delete[] path;
	}
}

void SocketServer::UDPBroadServerStart()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	char broadcast = '1';

	if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) != 0)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Setsockopt failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(ADDR_IP);
	server.sin_port = htons(PORT);

	if (::bind(m_socket, (sockaddr*)&server, sizeof(server)) != 0)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	int bytes, totalBytes, bufferNum, totalBufferNum;
	long file_size;
	int fileNameSize = 0;

	while (1)
	{
		int len = sizeof(server);
		char* buf = new char[SIZE];
		char* path = new char[PATH_SIZE];
		char nameBuf[100];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, SIZE);
		fileNameSize = recvfrom(m_socket, nameBuf, 20, 0, (SOCKADDR*)&server, &len);
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		file_size = atol(buf);
		totalBufferNum = file_size / SIZE + 1;
		bufferNum = 0;
		totalBytes = 0;

		errno_t ferr = fopen_s(&m_fp, path, "wb");

		if (ferr != 0)
		{
			m_eStr.Format(_T("File open error Code: %d"), ferr);
			AfxMessageBox(m_eStr);
			return;
		}

		int i = 0;
		while ((bytes = recvfrom(m_socket, buf, SIZE, 0, (SOCKADDR*)&server, &len)) != 0)
		{
			if (bytes == SOCKET_ERROR)
			{
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
				return;
			}

			bufferNum++;
			totalBytes += bufferNum;
			fwrite(buf, sizeof(char), bytes, m_fp);
		}
		fclose(m_fp);
		delete[] path;
	}
}

void SocketServer::UDPMultiServerStart()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// ���� ����ó��
	if (m_socket == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Create socket failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(ADDR_IP);
	server.sin_port = htons(PORT);

	if (::bind(m_socket, (const sockaddr*)&server, sizeof(server)))
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Bind failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.S_un.S_addr = inet_addr(ADDR_GROUP);
	mreq.imr_interface.S_un.S_addr = htonl(INADDR_ANY);

	if (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Setsockopt failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		return;
	}

	int bytes, totalBytes, bufferNum, totalBufferNum;
	long file_size;
	int fileNameSize = 0;

	while (1)
	{
		int len = sizeof(server);
		char* buf = new char[SIZE];
		char* path = new char[PATH_SIZE];
		char nameBuf[100];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		memset(buf, 0, SIZE);
		fileNameSize = recvfrom(m_socket, nameBuf, 20, 0, (SOCKADDR*)&server, &len);
		strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

		file_size = atol(buf);
		totalBufferNum = file_size / SIZE + 1;
		bufferNum = 0;
		totalBytes = 0;

		errno_t ferr = fopen_s(&m_fp, path, "wb");

		if (ferr != 0)
		{
			m_eStr.Format(_T("File open error Code: %d"), ferr);
			AfxMessageBox(m_eStr);
			return;
		}

		int i = 0;
		while ((bytes = recvfrom(m_socket, buf, SIZE, 0, (SOCKADDR*)&server, &len)) != 0)
		{
			if (bytes == SOCKET_ERROR)
			{
				m_eStr.Format(_T("File read error code: %d"), SOCKET_ERROR);
				AfxMessageBox(m_eStr);
				return;
			}

			bufferNum++;
			totalBytes += bufferNum;
			fwrite(buf, sizeof(char), bytes, m_fp);
		}
		fclose(m_fp);
		delete[] path;
	}
}

SocketServer::~SocketServer()
{
	if (m_fp != nullptr)
		fclose(m_fp);
	closesocket(m_socket);
	if (m_accept != NULL)
		closesocket(m_accept);
	WSACleanup();
}
// SocketServer member functions