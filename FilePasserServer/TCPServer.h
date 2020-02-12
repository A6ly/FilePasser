#pragma once
#include "SocketServer.h"

class TCPServer : public SocketServer
{
public:
	TCPServer(CListBox& logMessage, CProgressCtrl& fileProgress);
	~TCPServer();
	
	void TCPServerStart();
private:
	void createSocket();
	void bindSocket();
	void acceptSocket();
	void recvFile(SOCKET accept);
	SOCKET m_newSocket;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	int m_error;
};

