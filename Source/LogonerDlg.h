
// LogonerDlg.h : ͷ�ļ�
//

#pragma once


// CLogonerDlg �Ի���
class CLogonerDlg : public CDialogEx
{
// ����
public:
	CLogonerDlg(CWnd* pParent = NULL);	// ��׼���캯��

	enum { IDD = IDD_LOGONER_DIALOG };
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGONER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
