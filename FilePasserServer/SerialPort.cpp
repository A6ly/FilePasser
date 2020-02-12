#include "pch.h"
#include "SerialPort.h"

SerialPort::SerialPort(CListBox& logMessage, CProgressCtrl& fileProgress)
	: m_logMessage(logMessage), m_fileProgress(fileProgress), m_fp(nullptr)
{

}

BOOL SerialPort::openPort(CString portName)
{
	m_fileProgress.SetRange(0, 100);
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
	const char* del = "?";
	char* buf = new char[BUF_SIZE];

	BOOL readStatus, overlappedStatus;
	DWORD bytesRead;
	DWORD dwErrorFlags;
	COMSTAT comstat;
	OVERLAPPED overlapped;

	memset(&overlapped, 0, sizeof(OVERLAPPED));

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	time_t t = time(NULL);
	struct tm tm;
	errno_t err_t = localtime_s(&tm, &t);
	if (err_t != 0)
	{
		m_logMessage.AddString(L"Time ERROR!");
	}
	CString strTime;
	strTime.Format(L"%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	while (INFINITE_LOOP)
	{
		bytesRead = 600000;
		char* token = NULL;
		char* nextTokens = NULL;
		char path[PATH_SIZE] = "./";

		readStatus = ReadFile(m_hComport, buf, bytesRead, &bytesRead, &overlapped);
		while (overlappedStatus = GetOverlappedResult(m_hComport, &overlapped, &bytesRead, TRUE))
		{
			token = strtok_s(buf, "?", &nextTokens);
			int i = 0;
			while (token[i] != '\0') { ++i; }
			strncat_s(path, PATH_SIZE, token, i);

			errno_t err = fopen_s(&m_fp, path, "wb");
			if (err != 0)
			{
			#ifdef DEBUG
				  m_eStr.Format(_T("File open error Code: %d"), GetLastError());
				  AfxMessageBox(m_eStr);
			#else
				  AfxMessageBox(_T("File open error"));
			#endif
				  return;
			}

			WCHAR path_t[PATH_SIZE];
			size_t* size = new size_t;
			mbstowcs_s(size, path_t, PATH_SIZE, path, PATH_SIZE);

			dwErrorFlags = GetLastError();
			if (dwErrorFlags == ERROR_IO_INCOMPLETE)
			{
				fclose(m_fp);
				continue;
			}
			else
			{
				fwrite(nextTokens, sizeof(char), bytesRead, m_fp);
				ClearCommError(m_hComport, &dwErrorFlags, &comstat);
				m_fileProgress.SetPos(100);
				if ((AfxMessageBox(_T("Download complete"), MB_OK | MB_ICONINFORMATION) == IDOK))
				{
					m_fileProgress.SetPos(0);
				}
				m_logMessage.AddString(strTime);
				m_logMessage.AddString(L"Download file complete");
				m_logMessage.AddString(path_t);
			}
			fclose(m_fp);
			memset(buf, 0, BUF_SIZE);
			PurgeComm(m_hComport, PURGE_TXCLEAR | PURGE_RXCLEAR);
			break;
		}
		if (!overlappedStatus)
		{
			delete[] buf;
			ClearCommError(m_hComport, &dwErrorFlags, &comstat);
			PurgeComm(m_hComport, PURGE_TXABORT | PURGE_TXABORT);
			return;
		}
	}
}

SerialPort::~SerialPort()
{
	if (m_fp != nullptr)
	{
		fclose(m_fp);
	}
	CancelIo(m_hComport);
	PurgeComm(m_hComport, PURGE_TXCLEAR | PURGE_RXCLEAR);
	CloseHandle(m_hComport);
}