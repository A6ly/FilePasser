#include "pch.h"
#include "SocketClient.h"
#include "stdlib.h"
#include "stdio.h"
#include "WinSock2.h"

#define BUFSIZE 4096

#pragma warning(disable:4996)


void SocketClient::FileSend() {
	CFileDialog fdlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);

	if (IDOK == fdlg.DoModal()) {
		CString strPathName = fdlg.GetPathName();
		CFile fp;
		CFileException e;

		if (!fp.Open(strPathName, CFile::modeRead, &e)) {
			e.ReportError();
			return;
		}
		CString file;
		CArchive ar(&fp, CArchive::load);
		ar >> file;
	}





}
