#pragma once
#include "pch.h"
using namespace std;

class SocketServer
{
public:
	SocketServer();

	void TCPServerStart();

	void UDPServerStart();

	void UDPBroadServerStart();

	void UDPMultiServerStart();

	void initWinsock();

	void getLogMessage();

	virtual ~SocketServer();

protected:

private:
	CString m_eStr;
	SOCKET m_socket;
	SOCKET m_accept;
	FILE *m_fp;
	int m_error;
};


