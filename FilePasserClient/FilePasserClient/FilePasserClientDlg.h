// FilePasserClientDlg.h: 헤더 파일
//

#include "SocketClient.h"

#pragma once


// CFilePasserClientDlg 대화 상자
class CFilePasserClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CFilePasserClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	SocketClient m_SocketClient;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILEPASSERCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:


// 구현입니다.
public:


protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEnChangePortEdit();
	afx_msg void OnBnClickedLog();
	afx_msg void OnBnClickedButtonFilesend();
	afx_msg void OnBnClickedButtonConnect();
	CComboBox m_comboProtocolList;
	afx_msg void OnBnClickedButtonClose();
	CProgressCtrl m_progress;
};
