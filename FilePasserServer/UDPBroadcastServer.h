#pragma once
#include "SocketServer.h"
class UDPBroadcastServer : public SocketServer
{
public:
	UDPBroadcastServer(CListBox& logMessage, CProgressCtrl& fileProgress);
	~UDPBroadcastServer();

	void BroadcastStart();

private:
	void createSocket();
	void bindSocket();
	void recvFile();
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
};

