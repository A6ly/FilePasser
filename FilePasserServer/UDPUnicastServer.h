#pragma once
#include "SocketServer.h"

class UDPUnicastServer : public SocketServer
{
public:
	UDPUnicastServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime);
	~UDPUnicastServer();

	void UnicastStart();

private:
	void createSocket();
	void bindSocket();
	void recvFile();
	CString& m_strTime;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
};

