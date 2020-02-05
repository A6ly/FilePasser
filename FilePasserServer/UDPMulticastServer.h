#pragma once
#include "SocketServer.h"
class UDPMulticastServer : public SocketServer
{
public:
	UDPMulticastServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime);
	~UDPMulticastServer();

	void MulticastStart();

private:
	void createSocket();
	void bindSocket();
	void recvFile();
	SOCKET m_udpSocket;
	CString& m_strTime;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	struct ip_mreq mreq;
};

