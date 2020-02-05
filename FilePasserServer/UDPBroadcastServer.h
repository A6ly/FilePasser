#pragma once
#include "SocketServer.h"
class UDPBroadcastServer : public SocketServer
{
public:
	UDPBroadcastServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime);
	~UDPBroadcastServer();

	void BroadcastStart();

private:
	void createSocket();
	void bindSocket();
	void recvFile();
	CString& m_strTime;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
};

