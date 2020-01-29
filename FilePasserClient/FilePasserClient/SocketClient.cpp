// SocketClient.cpp: 구현 파일
//

#include "pch.h"
#include "FilePasserClient.h"
#include "SocketClient.h"


// SocketClient

SocketClient::SocketClient()
{
}

SocketClient::~SocketClient()
{
}


// SocketClient 멤버 함수

void SocketClient::SetWnd(HWND hWnd) {
	m_hWnd = hWnd;
}

void SocketClient::OnReceive(int nErrorCode) {
	TCHAR szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	if (Receive(szBuffer, sizeof(szBuffer)) > 0) {
		SendMessage(m_hWnd, WM_CLIENT_RECV, 0, (LPARAM)szBuffer);
	}

	CSocket::OnReceive(nErrorCode);

}

void SocketClient::OnClose(int nErrorCode) {
	ShutDown();
	Close();

	CSocket::OnClose(nErrorCode);

	AfxMessageBox(_T("Disconnected from Server!"), MB_OK | MB_ICONWARNING);
}