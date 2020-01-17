#pragma once
#include "pch.h"
using namespace std;

class SocketServer
{
public:
	SocketServer();

	BOOL TCPServerStart();

	void getMessage();

	virtual ~SocketServer();

protected:

private:
	int m_error;
	SOCKET m_TCPSocket;
};


