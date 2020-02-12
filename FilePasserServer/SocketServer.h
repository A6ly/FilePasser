#pragma once
#include "pch.h"
using namespace std;

typedef struct {
	char addr_ip[16];
	char addr_group_ip[16];
	int  port;
} network_config;

class SocketServer
{
public:
	SocketServer(CListBox& logMessage, CProgressCtrl& fileProgress);

	virtual ~SocketServer();

protected:
	FILE* m_recvfp;
	int m_error;
	CString m_eStr;
	SOCKET m_socket;
	struct sockaddr_in addr_server, addr_client;
	network_config* netconfig;
private:
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
};



