
// HupaiAssistantDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#define MIN_ADD_INTERVAL				100 // 最小加价量
#define PRICE_INTERVAL					300 // 接受出价区间

#define DISP_WIDTH						500
#define DISP_HEIGHT						400

#define CLICK_DELAY						50

//
// 热键定义
//
#define HOTKEY_OK						WM_USER + 1000
#define HOTKEY_CONFIRM					WM_USER + 1001
#define HOTKEY_CANCLE					WM_USER + 1002
#define HOTKEY_REFRESH					WM_USER + 1003
#define HOTKEY_CLEAR					WM_USER + 1004
#define HOTKEY_CHUJIA					WM_USER + 1005
#define HOTKEY_AUTO_CONFIRM				WM_USER + 1006
#define HOTKEY_ESCAPE					WM_USER + 1007
#define HOTKEY_TEST_YZM					WM_USER + 100


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
	CHotKeyCtrl	m_HotKey;				// 热键控件
	CButton		m_CalcHotKey;			// 计算热键值的按钮

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedHotkeyMode();
	afx_msg void OnBnClickedHotkeyCalc();
	DECLARE_MESSAGE_MAP()

private:

	CPen		penSolid;
	CPen		penDot;
	CBrush		brush;
	CFont		font;
	CFont		fontS;
	CFont		fontM;
	CFont		fontL;

	BOOL RegistHotKey();
	BOOL UnregisteHotKey();
	// static BOOL IEScreenShot(CDC *, CBitmap *, CRect *);

public:
	// IE相关信息
	static CWnd	*pWndIE;

	// 检测系统信息：系统时间、当前最低价
	static volatile BOOL canNormal;
	static volatile BOOL isNormal;
	static void Thread_Normal(void *);

	// 自动竞拍线程
	static volatile BOOL canAutoConfirm;
	static volatile BOOL isAutoConfirm;
	static void Thread_AutoConfirm(void *);

	// 测试：预览验证码（发送HOTKEY消息）
	// static void Thread_TestYZM(void *);
};
