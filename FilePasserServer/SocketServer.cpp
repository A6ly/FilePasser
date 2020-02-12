// SocketServer.cpp : implementation file
#pragma comment(lib, "Ws2_32.lib")
#include "pch.h"
#include "FilePasserServerDlg.h"
#include "SocketServer.h"
#include <string>
#include <filesystem>
#include <chrono>

#define MAX_CONNECTION 5
#define BUF_SIZE 1000000
#define SIZE 5000
#define FILE_NAME_SIZE 50
#define PATH_SIZE 100

// SocketServer
SocketServer::SocketServer(CListBox& logMessage, CProgressCtrl& fileProgress)
	: m_socket(NULL), m_error(0), m_recvfp(nullptr)
	, m_logMessage(logMessage), m_fileProgress(fileProgress)
{
	netconfig = (network_config*)malloc(sizeof(network_config));
	// Winsock √ ±‚»≠ 
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
	errno_t err_f = fopen_s(&config_fp, ".\\network_config.txt", "r");
	if (err_f != 0)
	{
		return;
	}

	fgets(netconfig->addr_ip, sizeof(char) * 16, config_fp);
	netconfig->addr_ip[strlen(netconfig->addr_ip) - 1] = '\0';
	fgets(netconfig->addr_group_ip, sizeof(char) * 16, config_fp);
	netconfig->addr_group_ip[strlen(netconfig->addr_group_ip) - 1] = '\0';
	fgets(port, sizeof(char) * 7, config_fp);
	netconfig->port = atoi(port);

	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.S_un.S_addr = inet_addr(netconfig->addr_ip);
	addr_server.sin_port = htons(netconfig->port);

	fclose(config_fp);
}

SocketServer::~SocketServer()
{
	closesocket(m_socket);
	free(netconfig);
	WSACleanup();
}
// SocketServer member functions