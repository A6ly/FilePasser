#include "pch.h"
#include "SerialPort.h"

SerialPort::SerialPort(CListBox& logMessage, CProgressCtrl& fileProgress) : m_logMessage(logMessage), m_fileProgress(fileProgress)
{
	
}

BOOL SerialPort::openPort(CString portName)
{
	m_hComport = CreateFile(
		portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL
	);

	if (m_hComport == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"Open COM failed!");
		return FALSE;
	}
	return TRUE;
}

FILE* fp;

BOOL SerialPort::configureSerialSet(DWORD baudRate, int dataByte, int stopbit, int paritybit)
{
	SetupComm(m_hComport, 1024, 1024);
	COMMTIMEOUTS timeOut;
	timeOut.ReadIntervalTimeout = 1000;
	timeOut.ReadTotalTimeoutMultiplier = 500;
	timeOut.ReadTotalTimeoutConstant = 5000;

	if (!SetCommTimeouts(m_hComport, &timeOut))
	{
		AfxMessageBox(L"Error: Unable to Set up Timeout");
		Sleep(1500);
		return FALSE;
	}

	GetCommState(m_hComport, &m_dcb);

	m_dcb.DCBlength = sizeof(m_dcb);
	m_dcb.BaudRate = baudRate;
	m_dcb.ByteSize = dataByte;
	m_dcb.StopBits = stopbit;
	m_dcb.Parity = paritybit;

	if (!SetCommState(m_hComport, &m_dcb))
	{
		AfxMessageBox(L"Error: Unable to Configure Serial port");
		Sleep(1500);
		return FALSE;
	}
	PurgeComm(m_hComport, PURGE_TXCLEAR | PURGE_RXCLEAR);

	return TRUE;
}

void SerialPort::readFile()
{
	PurgeComm(m_hComport, PURGE_TXCLEAR | PURGE_RXCLEAR);
	char* buf = new char[5000];
	char path[PATH_SIZE] = "C:/Users/user/Desktop/recv/";
	char nameBuf[FILE_NAME_SIZE] = { 0 };
	BOOL readStatus;
	DWORD bytesRead = 5000;
	DWORD dwErrorFlags;
	COMSTAT comstat;
	OVERLAPPED overlapped;
	
	memset(&overlapped, 0, sizeof(OVERLAPPED));

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
<<<<<<< HEAD
	
	while (INFINITE_LOOP)
	{
		bytesRead = 600000;
		char* token = NULL;
		char* nextTokens = NULL;
		char path[PATH_SIZE] = "C:/Users/user/Desktop/recv/";
		
		readStatus = ReadFile(m_hComport, buf, bytesRead, &bytesRead, &overlapped);
		if (readStatus == 0)
		{
			#ifdef DEBUG
				m_eStr.Format(_T("Read File error Code: %d"), GetLastError());
				AfxMessageBox(m_eStr);
			#else
				AfxMessageBox(_T("Can not open I/O port"));
			#endif
				return;
		}
		while (overlappedStatus = GetOverlappedResult(m_hComport, &overlapped, &bytesRead, TRUE))
		{
			token = strtok_s(buf, "?", &nextTokens);
			int i = 0;
			while (token[i] != '\0') { ++i; }
			strncat_s(path, PATH_SIZE, token, i);
=======
>>>>>>> d091b3f6c1230fa88ff7828b36003a1a60b9c45b

	
	readStatus = ReadFile(m_hComport, nameBuf, bytesRead, &bytesRead, &overlapped);
	GetOverlappedResult(m_hComport, &overlapped, &bytesRead, TRUE);
	strncat_s(path, PATH_SIZE, nameBuf, bytesRead);
	errno_t err = fopen_s(&fp, path, "wb");

	ClearCommError(m_hComport, &dwErrorFlags, &comstat);
	PurgeComm(m_hComport, PURGE_TXABORT | PURGE_RXABORT);

	bytesRead = 5000;
	ReadFile(m_hComport, buf, bytesRead, &bytesRead, &overlapped);
	if (!readStatus)
	{
		while (GetOverlappedResult(m_hComport, &overlapped, &bytesRead, TRUE))
		{
			dwErrorFlags = GetLastError();
			if (dwErrorFlags == ERROR_IO_INCOMPLETE)
				continue;
			else
			{
				fwrite(buf, sizeof(char), bytesRead, fp);
				ClearCommError(m_hComport, &dwErrorFlags, &comstat);
				break;
			}
			fwrite(buf, sizeof(char), bytesRead, fp);
		}
		
		delete[] buf;
		PurgeComm(m_hComport, PURGE_TXABORT | PURGE_RXABORT);
		SerialPort::~SerialPort();
		return;
	}
	else
	{
		ClearCommError(m_hComport, &dwErrorFlags, &comstat);
		delete[] buf;
		PurgeComm(m_hComport, PURGE_TXABORT | PURGE_RXABORT);
		return;
	}
}

SerialPort::~SerialPort()
{
	fclose(fp);
	CloseHandle(m_hComport);
}

