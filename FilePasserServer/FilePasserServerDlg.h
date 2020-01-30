
// FilePasserServerDlg.h : header file
//

#pragma once
#include "SocketServer.h"
#include <thread>

// CFilePasserServerDlg dialog
class CFilePasserServerDlg : public CDialogEx
{
// Construction
public:
	CFilePasserServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEPASSERSERVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
private:
	SocketServer* m_socketServer;

// Implementation
protected:
	HICON m_hIcon;
	CComboBox* m_combo_protocol;

private:
	int m_netType;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void LoadListBox();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox logMessage;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNMCustomdrawDownload(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedRadioSocket();
	afx_msg void OnBnClickedRadioRs232();
	afx_msg void OnCbnSelchangeComboProtocol();
	afx_msg void OnCbnSelchangeComboComprot();
	afx_msg void OnCbnSelchangeComboBaudrate();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnSelcancelServerLoglist();
	CProgressCtrl fileProgress;
};
