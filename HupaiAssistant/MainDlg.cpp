#include "stdafx.h"
#include "HupaiAssistant.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include <stack>
#include "tools.h"

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
volatile BOOL CMainDlg::canAutoConfirm = FALSE;
volatile BOOL CMainDlg::isCheckOCR = FALSE;
volatile BOOL CMainDlg::isAutoConfirm = FALSE;


CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	LOGFONT	lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lstrcpy(lf.lfFaceName, _T("Microsoft Yahei"));
	lf.lfHeight = -24;
	font.CreateFontIndirect(&lf);
	lf.lfHeight = -14;
	fontS.CreateFontIndirect(&lf);
	lf.lfHeight = -36;
	fontM.CreateFontIndirect(&lf);
	lf.lfHeight = -72;
	fontL.CreateFontIndirect(&lf);

	penOutline.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
}


CMainDlg::~CMainDlg() {

	font.DeleteObject();
	fontS.DeleteObject();
	fontM.DeleteObject();
	fontL.DeleteObject();

	penOutline.DeleteObject();
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

	//
	// 初始化窗口大小
	//
	CRect rClient, rWin;
	GetClientRect(&rClient);
	GetWindowRect(&rWin);
	rWin.DeflateRect(0, 0, rClient.Width(), rClient.Height());
	MoveWindow(0, 0, rWin.Width() + DISP_WIDTH, rWin.Height() + DISP_HEIGHT + 50);

	//
	// 启动OCR线程
	//
	canCheckOCR = TRUE;
	_beginthread(Thread_CheckOCR, 0, NULL);

	//
	// 注册系统热键
	//
	if(FALSE == RegistHotKey()) {
		AfxMessageBox(_T("注册系统热键出现问题。"));
		SendMessage(WM_CLOSE);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMainDlg::OnDestroy() {

	if(FALSE == UnregisteHotKey()) {
		AfxMessageBox(_T("解绑系统热键出现问题。"));
	}

	canCheckOCR = FALSE;
	canAutoConfirm = FALSE;
	while(isCheckOCR || isAutoConfirm) {
		Sleep(500);
	}

	CDialogEx::OnDestroy();
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


static inline void ClickOnce(CPoint &pt) {
	CPoint ptMap, ptSys(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	ptMap.x = (DWORD)((FLOAT)pt.x / ptSys.x * 65535);
	ptMap.y = (DWORD)((FLOAT)pt.y / ptSys.y * 65535);
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, ptMap.x, ptMap.y, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTDOWN, ptMap.x, ptMap.y, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, ptMap.x, ptMap.y, 0, 0);
}

static inline void ClearInput(CPoint &pt) {
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('A', 0, 0, 0);
	keybd_event('A', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_DELETE, 0, 0, 0);
	keybd_event(VK_DELETE, 0, KEYEVENTF_KEYUP, 0);
}

static inline void InputChar(BYTE c) {
	keybd_event(c, 0, 0, 0);
	keybd_event(c, 0, KEYEVENTF_KEYUP, 0);
}


void CMainDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2) {

	CWnd *pWndIE = NULL;
	CPoint pt, ptIndex(theApp.settings.pt_index);


	if(pWndIE = FindWindow(_T("IEFrame"), NULL)) {
		pWndIE->ClientToScreen(&ptIndex);
	} else {
		AfxMessageBox(_T("未找到IE浏览器。"));
		return;
	}

	if(nHotKeyId == HOTKEY_OK) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_ok);
		ClickOnce(pt);

	} else if(nHotKeyId == HOTKEY_CONFIRM) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_ok);
		ClickOnce(pt);

	} else if(nHotKeyId == HOTKEY_CANCLE) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_cancle);
		ClickOnce(pt);

	} else if(nHotKeyId == HOTKEY_REFRESH) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_refresh);
		ClickOnce(pt);

	} else if(nHotKeyId == HOTKEY_CLEAR) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_input);
		ClearInput(pt);

	} else if(nHotKeyId == HOTKEY_CHUJIA) {

		if(theApp.status.autoBidStep == 0) {
			theApp.status.autoBidStep = 1;
		} else {
			return;
		}

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_jiajia_input);
		ClickOnce(pt);
		Sleep(CLICK_DELAY * 4);
		ClearInput(pt);
		Sleep(CLICK_DELAY);
		
		int delta = theApp.settings.bid.add_price;
		std::stack<BYTE> stk;
		while(delta > 0) {
			stk.push(delta % 10);
			delta /= 10;
		}
		while(!stk.empty()) {
			InputChar(stk.top() + '0');
			stk.pop();
		}

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_jiajia);
		ClickOnce(pt);
		Sleep(CLICK_DELAY * 8);
		pt = ptIndex;
		pt.Offset(theApp.settings.pt_chujia);
		ClickOnce(pt);

	} else if(nHotKeyId == HOTKEY_AUTO_CONFIRM) {

		if(theApp.status.autoBidStep == 1) {
			theApp.status.autoBidStep = 2;
		} else {
			return;
		}
		canAutoConfirm = TRUE;
		_beginthread(Thread_AutoConfirm, 0, NULL);

	} else if(nHotKeyId == HOTKEY_ESCAPE) {

		canAutoConfirm = FALSE;
		theApp.status.autoBidStep = 0;

	} else {
		AfxMessageBox(_T("未识别的热键！"));
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


BOOL CMainDlg::OnEraseBkgnd(CDC* pDC) {

	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// return CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
}


void CMainDlg::OnPaint() {

	CPaintDC dc(this);
	CDC memDC;
	CBitmap memBmp;
	CRect rect(0, 0, DISP_WIDTH, DISP_HEIGHT);
	CString str;
	SYSTEMTIME time;

	memDC.CreateCompatibleDC(&dc);
	memBmp.CreateCompatibleBitmap(&dc, DISP_WIDTH, DISP_HEIGHT);
	memDC.SelectObject(&memBmp);
	memDC.SetBkMode(TRANSPARENT);

	memDC.FillSolidRect(&rect, RGB(0, 0, 40));
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(40, 220, 40));
	DeleteObject(memDC.SelectObject(penOutline));
	DeleteObject(memDC.SelectObject(font));

	GetLocalTime(&time);
	str.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
	memDC.SelectObject(fontL);
	memDC.TextOut(20, 20, str);
	str.Format(_T("%02d"), time.wMilliseconds / 10);
	memDC.SelectObject(fontM);
	memDC.TextOut(320, 58, str);
	memDC.SetTextColor(RGB(80, 80, 160));
	str.Format(_T("%.2f"), (float)theApp.status.serverDelay / 1000);
	memDC.TextOut(390, 58, str);
	memDC.SetTextColor(RGB(40, 220, 40));

	if(theApp.status.autoBidStep == 1) {
		CRect rgnInfo = theApp.settings.rgn_yzm_info;
		CRect rgnPic = theApp.settings.rgn_yzm_picture;
		rgnInfo.OffsetRect(theApp.settings.pt_index);
		rgnPic.OffsetRect(theApp.settings.pt_index);
		if(CDC *pDcIE = FindWindow(_T("IEFrame"), NULL)->GetDC()) {
			memDC.BitBlt(20, 200, rgnInfo.Width(), rgnInfo.Height(), 
				pDcIE, rgnInfo.left, rgnInfo.top, SRCCOPY);
			memDC.StretchBlt(20, 240, rgnPic.Width()*3, rgnPic.Height()*3, 
				pDcIE, rgnPic.left,rgnPic.top, rgnPic.Width(), rgnPic.Height(), SRCCOPY);
		}
	} else if(theApp.status.autoBidStep == 2) {

	}

	dc.BitBlt(0, 0, DISP_WIDTH, DISP_HEIGHT, &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
	memBmp.DeleteObject();
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

BOOL CMainDlg::RegistHotKey() {

	HWND hWnd = this->GetSafeHwnd();
	BOOL flag = TRUE;
	DWORD key;


	if(key = theApp.settings.hotkey_ok) {
		flag &= RegisterHotKey(hWnd, HOTKEY_OK, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_confirm) {
		flag &= RegisterHotKey(hWnd, HOTKEY_CONFIRM, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_cancle) {
		flag &= RegisterHotKey(hWnd, HOTKEY_CANCLE, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_refresh) {
		flag &= RegisterHotKey(hWnd, HOTKEY_REFRESH, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_clear) {
		flag &= RegisterHotKey(hWnd, HOTKEY_CLEAR, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_chujia) {
		flag &= RegisterHotKey(hWnd, HOTKEY_CHUJIA, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hoteky_auto_confirm) {
		flag &= RegisterHotKey(hWnd, HOTKEY_AUTO_CONFIRM, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hoteky_escape) {
		flag &= RegisterHotKey(hWnd, HOTKEY_ESCAPE, HIWORD(key), LOWORD(key));
	}
	
	return flag;
}


BOOL CMainDlg::UnregisteHotKey() {

	HWND hWnd = this->GetSafeHwnd();
	BOOL flag = TRUE;


	if(theApp.settings.hotkey_ok) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_OK);
	}
	if(theApp.settings.hotkey_confirm) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_CONFIRM);
	}
	if(theApp.settings.hotkey_cancle) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_CANCLE);
	}
	if(theApp.settings.hotkey_refresh) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_REFRESH);
	}
	if(theApp.settings.hotkey_clear) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_CLEAR);
	}
	if(theApp.settings.hotkey_chujia) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_CHUJIA);
	}
	if(theApp.settings.hoteky_auto_confirm) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_AUTO_CONFIRM);
	}
	if(theApp.settings.hoteky_escape) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_ESCAPE);
	}

	return flag;
}


//==================================================================================================


void CMainDlg::Thread_CheckOCR(void *param) {

	CDC *pDcIE = NULL, dc;
	CBitmap bitmap;
	CRect rect;
	BYTE mask[512];
	SYSTEMTIME time;
	int ocr;


	isCheckOCR = TRUE;
	while(canCheckOCR) {
		
		if(pDcIE = FindWindow(_T("IEFrame"), NULL)->GetDC()) {
			dc.CreateCompatibleDC(pDcIE);
		} else {
			AfxMessageBox(_T("未找到IE浏览器。"));
			theApp.GetMainWnd()->SendMessage(WM_CLOSE);
			break;
		}
		
		rect = theApp.settings.rgn_ocr_time;
		rect.OffsetRect(theApp.settings.pt_index);
		bitmap.CreateCompatibleBitmap(pDcIE, rect.Width(), rect.Height()); // 用&dc不行！
		dc.SelectObject(&bitmap);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), pDcIE, rect.left, rect.top, SRCCOPY);
		Tools::SaveBitmap(&dc, "./ss_time.bmp");
		Tools::GetBitmapMask(mask, &dc);
		bitmap.DeleteObject();
		ocr = Tools::OCR_Number(mask, rect.Width());
		theApp.status.serverHour = (ocr / 10000) % 100;
		theApp.status.serverMinute = (ocr / 100) % 100;
		theApp.status.serverSecond = ocr % 100;
		GetLocalTime(&time);
		theApp.status.serverDelay = ((time.wSecond - theApp.status.serverSecond + 60) % 60) * 1000 + time.wMilliseconds;

		rect = theApp.settings.rgn_ocr_price;
		rect.OffsetRect(theApp.settings.pt_index);
		bitmap.CreateCompatibleBitmap(pDcIE, rect.Width(), rect.Height());
		dc.SelectObject(&bitmap);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), pDcIE, rect.left, rect.top, SRCCOPY);
		Tools::SaveBitmap(&dc, "./ss_price.bmp");
		Tools::GetBitmapMask(mask, &dc);
		bitmap.DeleteObject();
		ocr = Tools::OCR_Number(mask, rect.Width());
		theApp.status.price = ocr;
		
		dc.DeleteDC();
		theApp.GetMainWnd()->Invalidate();
		Sleep(50);
	}
	isCheckOCR = FALSE;
}


void CMainDlg::Thread_AutoConfirm(void *param) {

	SYSTEMTIME time;
	int current, commit = theApp.settings.bid.commit_before % 60000;


	isAutoConfirm = TRUE;

	do {
		if(theApp.settings.isRealMode) {
			GetSystemTime(&time);
			current = time.wSecond * 1000 + time.wMilliseconds;
		} else {
			current = theApp.status.serverSecond * 1000;
		}
		Sleep(10);
	} while(current < commit);
	theApp.GetMainWnd()->SendMessage(WM_HOTKEY, HOTKEY_CONFIRM, theApp.settings.hotkey_confirm);

	canAutoConfirm = FALSE;
	isAutoConfirm = FALSE;
}

//==================================================================================================
