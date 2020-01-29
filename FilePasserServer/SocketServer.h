#pragma once
#include "pch.h"
#include "FilePasserServer.h"
#include "FilePasserServerDlg.h"
using namespace std;

class CFilePasserServerDlg;

class SocketServer
{
public:
	SocketServer(CListBox& logMessage, CProgressCtrl& fileProgress);

	void TCPServerStart();

	void UDPServerStart();

	void UDPBroadServerStart();

	void UDPMultiServerStart();

	void initWinsock();

	virtual ~SocketServer();

protected:

private:
	CString m_eStr;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	SOCKET m_socket;
	SOCKET m_accept;
	FILE *m_fp;
	int m_error;
};


