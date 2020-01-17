// SocketServer.cpp : implementation file
#pragma comment(lib, "Ws2_32.lib")
#include "pch.h"
#include "FilePasserServer.h"
#include "SocketServer.h"
#define PORT 2020
#define MAX_CONNECTION 5

// SocketServer
SocketServer::SocketServer() : m_error(0), m_TCPSocket(NULL)
{
	WSADATA wsa;
	m_error = WSAStartup(MAKEWORD(2, 2), &wsa);					// Winsock �ʱ�ȭ
	if (m_error != 0)
	{
		m_error = WSAGetLastError();
		cout << "WSA initialization Error Code: " << m_error << endl;
		return;
	}
}

// Start ��ư Ŭ�� �� ���� ����
BOOL SocketServer::TCPServerStart()
{
	m_TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);			// TCP���� ����
	if (m_TCPSocket == INVALID_SOCKET)									// ���� ����ó��
	{
		m_error = WSAGetLastError();
		cout << "Create Socket failed Error Code: " << m_error << endl;
		return false;
	}
	
	struct sockaddr_in server , client;									// ���� �ּ� �ʱ�ȭ
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	if (bind(m_TCPSocket, (sockaddr*)&server, sizeof(server)) != 0)		// ���� ���ε�
	{
		m_error = WSAGetLastError();
		cout << "Bind failed Error Code: " << WSAGetLastError() << endl;
		return false;
	}

	if (listen(m_TCPSocket, SOMAXCONN_HINT(8) != 0))					// ���� ����
	{
		m_error = WSAGetLastError();
		cout << "Listen failed Error Code: " << WSAGetLastError() << endl;
		return false;
	}

	SOCKET accpetSocket;
	int clientLen = sizeof(struct sockaddr_in);
	accpetSocket = accept(m_TCPSocket, (sockaddr*)&client, &clientLen);	// ���� Accept
	if (accpetSocket == INVALID_SOCKET)
	{
		m_error = WSAGetLastError();
		cout << "Accept failed Error Code: " << m_error << endl;
		return false;
	}

	return true;
}

SocketServer::~SocketServer()
{
	closesocket(m_TCPSocket);
	WSACleanup();
}
// SocketServer member functions