#include "stdafx.h"
#include "HupaiAssistant.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//
// Class: CAboutDlg
//

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


//
// Class: CMainDlg
//

volatile BOOL CMainDlg::canCheckOCR = FALSE;
volatile BOOL CMainDlg::canBltYZM = FALSE;
volatile BOOL CMainDlg::canAutoConfirm = FALSE;
volatile BOOL CMainDlg::isCheckOCR = FALSE;
volatile BOOL CMainDlg::isBltYZM = FALSE;
volatile BOOL CMainDlg::isAutoConfirm = FALSE;


CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CMainDlg::~CMainDlg() {

}


BOOL CMainDlg::OnInitDialog() {

	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rClient, rWin;
	GetClientRect(&rClient);
	GetWindowRect(&rWin);
	rWin.DeflateRect(0, 0, rClient.Width(), rClient.Height());
	MoveWindow(0, 0, rWin.Width() + DISP_WIDTH, rWin.Height() + DISP_HEIGHT + 50);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMainDlg::OnDestroy() {

	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg) {

	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMainDlg::DoDataExchange(CDataExchange* pDX) {

	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY_MODE, m_EnableHotKey);
	DDX_Check(pDX, IDC_HOTKEY_MODE, m_HotKeyEnabled);
	DDX_Control(pDX, IDC_HOTKEY_TEST, m_HotKey);
	DDX_Control(pDX, IDC_HOTKEY_CALC, m_CalcHotKey);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_HOTKEY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_HOTKEY_MODE, &CMainDlg::OnBnClickedHotkeyMode)
	ON_BN_CLICKED(IDC_HOTKEY_CALC, &CMainDlg::OnBnClickedHotkeyCalc)
END_MESSAGE_MAP()


void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam) {

	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CMainDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2) {

	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


BOOL CMainDlg::OnEraseBkgnd(CDC* pDC) {

	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// return CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
}


void CMainDlg::OnPaint() {

	CPaintDC dc(this);
}


HCURSOR CMainDlg::OnQueryDragIcon() {

	return static_cast<HCURSOR>(m_hIcon);
}


void CMainDlg::OnBnClickedHotkeyMode() {

	// TODO: 在此添加控件通知处理程序代码
}


void CMainDlg::OnBnClickedHotkeyCalc() {

	// TODO: 在此添加控件通知处理程序代码
}


//==================================================================================================

void CMainDlg::_RegistHotKey() {


}


void CMainDlg::_UnregisteHotKey() {


}


void CMainDlg::RegistHotKey() {


}


void CMainDlg::UnregisteHotKey() {


}


//==================================================================================================


void CMainDlg::Thread_CheckOCR(void *param) {


}


void CMainDlg::Thread_BltYZM(void *param) {


}


void CMainDlg::Thread_AutoConfirm(void *param) {


}

//==================================================================================================
