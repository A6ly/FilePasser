#pragma once
#include "pch.h"
#include "FilePasserServer.h"
#include "FilePasserServerDlg.h"
using namespace std;


class SocketServer : public CDialogEx
{
public:
	SocketServer(CListBox& logMessage, CProgressCtrl& fileProgress, CString& strTime);

	void TCPServerStart();

	void UDPServerStart();

	void UDPBroadServerStart();

	void UDPMultiServerStart();

	virtual ~SocketServer();

protected:

private:
	CString m_eStr;
	CString& m_strTime;
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	SOCKET m_socket;
	SOCKET m_accept;
	FILE *m_fp;
	int m_error;
};


