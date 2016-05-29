
// HupaiAssistantDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


#define DISP_WIDTH						500
#define DISP_HEIGHT						300

//
// 热键定义
//
#define HOTKEY_START					WM_USER + 1000
#define HOTKEY_OK						WM_USER + 1001
#define HOTKEY_YZM_REFRESH				WM_USER + 1002
#define HOTKEY_YZM_CLEAR				WM_USER + 1003
#define HOTKEY_YZM_OK					WM_USER + 1004
#define HOTKEY_YZM_CANCLE				WM_USER + 1005
#define HOTKEY_ENTER					WM_USER + 1006
#define HOTKEY_ESCAPE					WM_USER + 1007


// CHupaiAssistantDlg 对话框
class CMainDlg : public CDialogEx
{
public:
	CMainDlg(CWnd* pParent = NULL);
	~CMainDlg();

protected:
	enum { IDD = IDD_MAIN_DIALOG };
	HICON m_hIcon;

	CButton		m_EnableHotKey;			// 打开系统热键？
	BOOL		m_HotKeyEnabled;		// 
	CHotKeyCtrl	m_HotKey;				// 热键控件
	CButton		m_CalcHotKey;			// 计算热键值的按钮

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	DECLARE_MESSAGE_MAP()

private:
	CRect		rgnDisp;

	CPen		penOutline;
	CFont		font;
	CFont		fontS;
	CFont		fontM;
	CFont		fontL;

	void _RegistHotKey();
	void _UnregisteHotKey();
	void RegistHotKey();
	void UnregisteHotKey();
	// static BOOL IEScreenShot(CDC *, CBitmap *, CRect *);

public:
	// 检测系统信息：系统时间、当前最低价
	static volatile BOOL canCheckOCR;
	static volatile BOOL isCheckOCR;
	static void Thread_CheckOCR(void *);
	// 拷贝验证码
	static volatile BOOL canBltYZM;
	static volatile BOOL isBltYZM;
	static void Thread_BltYZM(void *);
	// 自动竞拍线程
	static volatile BOOL canAutoConfirm;
	static volatile BOOL isAutoConfirm;
	static void Thread_AutoConfirm(void *);
	afx_msg void OnBnClickedHotkeyMode();
	afx_msg void OnBnClickedHotkeyCalc();
};
