#pragma once
#include "SocketServer.h"

class TCPServer : public SocketServer
{
public:
	TCPServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime);
	~TCPServer();
	
	void TCPServerStart();
private:
	void createSocket();
	void bindSocket();
	void acceptSocket();
	void recvFile(SOCKET accept);
	SOCKET m_tcpSocket;
	SOCKET m_newSocket;
	CString m_eStr;
	CString& m_strTime;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	int m_error;
};

