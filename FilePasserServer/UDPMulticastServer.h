#pragma once
#include "SocketServer.h"
class UDPMulticastServer : public SocketServer
{
public:
	UDPMulticastServer(CListBox& logMessage, CProgressCtrl& fileProgress);
	~UDPMulticastServer();

	void MulticastStart();

private:
	void createSocket();
	void bindSocket();
	void recvFile();
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	struct ip_mreq mreq;
};

