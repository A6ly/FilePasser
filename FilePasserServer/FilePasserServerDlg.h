
// FilePasserServerDlg.h : header file
//

#pragma once
#include "SocketServer.h"
#include "SerialPort.h"
#include "TCPServer.h"
#include "UDPUnicastServer.h"
#include "UDPBroadcastServer.h"
#include "UDPMulticastServer.h"
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
	SerialPort* m_serialPort;

// Implementation
protected:
	HICON m_hIcon;
	CComboBox* m_combo_protocol, * m_combo_baudrate, * m_combo_databit,
		* m_combo_stopbit, * m_combo_paritybit, * m_combo_portName;

private:
	int m_netType;
	int m_baudrate, m_databit, m_stopbit, m_paritybit;
	CString m_strTime;
	TCPServer* m_tcpServer;
	UDPUnicastServer* m_udpUnicastServer;
	UDPBroadcastServer* m_udpBroadcastServer;
	UDPMulticastServer* m_udpMulticastServer;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void LoadListBox();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void SaveLog();
	CListBox logMessage;
	CProgressCtrl fileProgress;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNMCustomdrawDownload(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedRadioSocket();
	afx_msg void OnBnClickedRadioRs232();
	afx_msg void OnCbnSelchangeComboProtocol();
	afx_msg void OnCbnSelchangeComboBaudrate();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnCbnSelchangeComboParitybit();
	afx_msg void OnCbnSelchangeComboStopbit();
	afx_msg void OnCbnSelchangeComboDatabit();
	afx_msg void OnBnClickedButtonClose();
};
