#pragma once
#include "SocketServer.h"

class UDPUnicastServer : public SocketServer
{
public:
	UDPUnicastServer(CListBox& logMessage, CProgressCtrl& fileProgress);
	~UDPUnicastServer();

	void UnicastStart();

private:
	void createSocket();
	void bindSocket();
	void recvFile();
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
};

