#pragma once

#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;

class SerialPort
{
public:
	SerialPort(CListBox& logMessage, CProgressCtrl& fileProgress);
	virtual ~SerialPort(void);

private:
	CListBox& m_logMessage;
	CProgressCtrl& m_fileProgress;
	HANDLE m_hComport;
	DCB m_dcb;

public:
	BOOL openPort(CString portName);
	BOOL configureSerialSet(DWORD baudRate, int dataByte, int stopbit, int paritybit);
	void readFile();
};

