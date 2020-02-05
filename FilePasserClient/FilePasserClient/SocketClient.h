#pragma once

// SocketClient 명령 대상

#define WM_CLIENT_RECV WM_USER+1

class SocketClient : public CSocket
{
public:
	SocketClient(CListBox& logMessage);
	virtual ~SocketClient();
	void SetWnd(HWND hWnd);
	HWND m_hWnd;
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	

private:
	CListBox& m_logMessage;
};


