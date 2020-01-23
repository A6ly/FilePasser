// SocketServer.cpp : implementation file
#pragma comment(lib, "Ws2_32.lib")
#include "pch.h"
#include "FilePasserServer.h"
#include "SocketServer.h"
#include <string>
#include <filesystem>
#include <chrono>

#define PORT 2020
#define MAX_CONNECTION 5
#define IP_ADDR "192.168.0.27"
#define BUF_SIZE 1000000
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
	server.sin_addr.S_un.S_addr = inet_addr(IP_ADDR);
	server.sin_family = AF_INET;
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

	//SOCKET static acceptSocket;
	int clientLen = sizeof(struct sockaddr_in);

	// ���� Accept
	m_accept = accept(m_socket, (sockaddr*)&client, &clientLen);
	if (m_accept == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		m_eStr.Format(_T("Accept failed Error Code: %d"), m_error);
		AfxMessageBox(m_eStr);
		closesocket(m_accept);
		return;
	}

	/* ������ ���� ����Ʈ ũ��� ���� ũ�� */
	int bytes, totalBytes, bufferNum, totalBufferNum;
	long file_size;
	int fileNameSize = 0;
	//futureObj.wait_for(chrono::microseconds(1)) == future_status::timeout
	while (1)
	{
		char* path = new char[PATH_SIZE];

		strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
		char* buf = new char[BUF_SIZE];
		char nameBuf[20];
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
				return;
			}
			else
			{
				m_eStr.Format(_T("client disconnected server"));
				AfxMessageBox(m_eStr);
				exit(1);
			}
		}	
		// ���� ���� ���ۿ� ��� �Ҵ� ����
		fclose(m_fp);
		delete[] path;
	}

	SocketServer::~SocketServer();
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
	server.sin_addr.S_un.S_addr = inet_addr(IP_ADDR);
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

	int len;
	len = sizeof(client);
	char* buf = new char[BUF_SIZE];
	char* path = new char[PATH_SIZE];
	char nameBuf[20];

	strcpy_s(path, PATH_SIZE, "C:/Users/user/Desktop/recv/");
	memset(buf, 0, BUF_SIZE);
	fileNameSize = recvfrom(m_socket, nameBuf, 20, 0, (SOCKADDR*)&client, &len);
	strncat_s(path, PATH_SIZE, nameBuf, fileNameSize);

	file_size = atol(buf);
	totalBufferNum = file_size / BUF_SIZE + 1;
	bufferNum = 0;
	totalBytes = 0;

	errno_t ferr = fopen_s(&m_fp, path, "wb");

	if (ferr != 0)
	{
		m_eStr.Format(_T("File open error Code: %d"), ferr);
		AfxMessageBox(m_eStr);
		return;
	}

	while (totalBufferNum != bufferNum)
	{
		if ((bytes = recvfrom(m_socket, buf, BUF_SIZE, 0, (SOCKADDR*)&client, &len)) != 0)
		{
			bufferNum++;
			totalBufferNum += bufferNum;
			fwrite(buf, sizeof(char), bytes, m_fp);
		}

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