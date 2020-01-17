
// FilePasserServerDlg.h : header file
//

#pragma once


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


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnLbnSelchangeServer();
	afx_msg void OnNMCustomdrawDownload(NMHDR* pNMHDR, LRESULT* pResult);
};
