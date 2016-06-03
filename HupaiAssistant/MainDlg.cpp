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

	penSolid.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	penDot.CreatePen(PS_DOT, 1, RGB(0, 255, 0));

	brush.CreateSolidBrush(RGB(40, 220, 40));
}


CMainDlg::~CMainDlg() {

	font.DeleteObject();
	fontS.DeleteObject();
	fontM.DeleteObject();
	fontL.DeleteObject();

	penSolid.DeleteObject();
	penDot.DeleteObject();

	brush.DeleteObject();
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
	// 注册系统热键
	//
	if(FALSE == RegistHotKey()) {
		AfxMessageBox(_T("注册系统热键出现问题。"));
		SendMessage(WM_CLOSE);
	}

	//
	// IE相关
	//
	if(pWndIE = CDialog::FindWindow(_T("IEFrame"), NULL)) {
		if(IDOK == MessageBox(_T("检测到IE已经打开，是否重新启动？"), _T("提示"), MB_OKCANCEL)) {
			pWndIE->SendMessage(WM_CLOSE);
			pWndIE = NULL;
		}
	}
	if(NULL == pWndIE) {
		ShellExecute(NULL, _T("open"), _T("C:\\Program Files\\Internet Explorer\\iexplore.exe"), theApp.settings.ie_url, NULL, SW_SHOWNORMAL);
		do {
			Sleep(1000);
		} while(NULL == (pWndIE = CDialog::FindWindow(_T("IEFrame"), NULL)));
	}
	HWND hWnd = pWndIE->GetSafeHwnd();
	::SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~(WS_MINIMIZEBOX  | WS_MAXIMIZEBOX | WS_THICKFRAME) );
	::SetWindowPos(hWnd, NULL, 0, 0, theApp.settings.ie_width, theApp.settings.ie_height, SWP_NOMOVE | SWP_FRAMECHANGED);

	//
	// 启动OCR线程
	//
	canNormal = TRUE;
	_beginthread(Thread_Normal, 0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMainDlg::OnDestroy() {

	if( (! m_EnableHotKey.GetCheck()) && (UnregisteHotKey() == FALSE) ) {
		AfxMessageBox(_T("解绑系统热键出现问题。"));
	}

	canNormal = FALSE;
	canAutoConfirm = FALSE;
	while(isNormal || isAutoConfirm) {
		Sleep(500);
	}

	CDialogEx::OnDestroy();
}


void CMainDlg::DoDataExchange(CDataExchange* pDX) {

	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY_MODE, m_EnableHotKey);
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

	CPoint pt, ptIndex(theApp.settings.pt_index);


	pWndIE->ClientToScreen(&ptIndex);

	if(nHotKeyId == HOTKEY_OK) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_ok);
		ClickOnce(pt);
		theApp.status.autoBidStep = Status::NORMAL;

	} else if(nHotKeyId == HOTKEY_CONFIRM) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_ok);
		ClickOnce(pt);
		theApp.status.autoBidStep = Status::NORMAL;

	} else if(nHotKeyId == HOTKEY_CANCLE) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_cancle);
		ClickOnce(pt);
		theApp.status.autoBidStep = Status::NORMAL;

	} else if(nHotKeyId == HOTKEY_REFRESH) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_refresh);
		ClickOnce(pt);

	} else if(nHotKeyId == HOTKEY_CLEAR) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_input);
		ClearInput(pt);

	} else if(nHotKeyId == HOTKEY_CHUJIA) {

		if(theApp.status.autoBidStep == Status::NORMAL) {
			theApp.status.autoBidStep = Status::YZM;
		} else {
			return;
		}

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_jiajia_input);
		ClickOnce(pt);
		Sleep(CLICK_DELAY);
		ClickOnce(pt); // Click Twice to ensure this click.
		Sleep(CLICK_DELAY);

		ClearInput(pt);
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
		Sleep(CLICK_DELAY);

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_jiajia);
		ClickOnce(pt);
		Sleep(CLICK_DELAY);
		pt = ptIndex;
		pt.Offset(theApp.settings.pt_chujia);
		ClickOnce(pt);

		theApp.status.bid_price = theApp.status.price + theApp.settings.bid.add_price;

	} else if(nHotKeyId == HOTKEY_AUTO_CONFIRM) {

		if(theApp.status.autoBidStep != Status::YZM) {
			return;
		}
		Tools::SaveBitmap(pWndIE->GetDC(), "ss_shot.bmp");
		theApp.status.autoBidStep = Status::AUTO_CONFIRM;
		canAutoConfirm = TRUE;
		_beginthread(Thread_AutoConfirm, 0, NULL);

	} else if(nHotKeyId == HOTKEY_ESCAPE) {

		// 点击确认
		pt = ptIndex;
		pt.Offset(theApp.settings.pt_ok);
		ClickOnce(pt);
		Sleep(CLICK_DELAY);
		// 点击取消
		pt = ptIndex;
		pt.Offset(theApp.settings.pt_yzm_cancle);
		ClickOnce(pt);
		Sleep(CLICK_DELAY);
		// 点击确认
		pt = ptIndex;
		pt.Offset(theApp.settings.pt_ok);
		ClickOnce(pt);
		// 结束智能出价线程（如果开启），回归常规模式。
		if(theApp.status.autoBidStep != Status::NORMAL) {
			canAutoConfirm = FALSE;
			theApp.status.autoBidStep = Status::NORMAL;
		}

	} else if(nHotKeyId == HOTKEY_TEST_YZM) {

		pt = ptIndex;
		pt.Offset(theApp.settings.pt_chujia);
		ClickOnce(pt);
		// Sleep(3000);
		// pt = ptIndex;
		// pt.Offset(theApp.settings.pt_yzm_cancle);
		// ClickOnce(pt);

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


static inline void FillTriangle(CDC *pDc, POINT &pt, BOOL top=FALSE) {

	POINT gpt[4] = {pt.x, pt.y, pt.x - 10, pt.y + 10, pt.x + 10, pt.y + 10, pt.x, pt.y};
	if(top) { gpt[1].y -= 20; gpt[2].y -= 20; }
	pDc->Polygon(gpt, 4);
}


void CMainDlg::OnPaint() {

	CPaintDC dc(this);
	CDC memDC;
	CBitmap memBmp;
	SYSTEMTIME time;
	CString str;
	CRect rect(0, 0, DISP_WIDTH, DISP_HEIGHT);
	CPoint pt;
	int steps, nStep;
	COLORREF bkgnd = RGB(0, 0, 40);
	COLORREF green = RGB(40, 220, 40);
	COLORREF violet = RGB(80,80, 160);
	COLORREF red = RGB(220, 40, 40);
	COLORREF gray = RGB(80, 80, 80);
#define MARGIN		20
#define Y_TIME		10
#define Y_GRAPH		140
#define Y_YZM		200
#define H_GRAPH		20
#define L_GRID		10


	//
	// Init DC
	//
	memDC.CreateCompatibleDC(&dc);
	memBmp.CreateCompatibleBitmap(&dc, DISP_WIDTH, DISP_HEIGHT);
	memDC.SelectObject(&memBmp);
	memDC.SetBkMode(TRANSPARENT);
	memDC.FillSolidRect(&rect, bkgnd);

	//
	// Time
	//
	if(theApp.settings.isRealMode) {
		GetLocalTime(&time);
		str.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
	} else {
		time.wMilliseconds = 0;
		str.Format(_T("%02d:%02d:%02d"), theApp.status.serverHour, theApp.status.serverMinute, theApp.status.serverSecond);
	}
	memDC.SelectObject(fontL);
	memDC.SetTextColor(green);
	memDC.TextOut(MARGIN, Y_TIME, str);
	str.Format(_T("%02d"), time.wMilliseconds / 10);
	memDC.SelectObject(fontM);
	memDC.TextOut(MARGIN + 300, Y_TIME + 38, str);
	str.Format(_T("Δ%+.2f"), (float)theApp.status.serverDelay / 1000);
	memDC.SelectObject(font);
	memDC.SetTextColor(violet);
	memDC.TextOut(MARGIN + 360, Y_TIME + 40, str);

	//
	// Price Graph
	//
	memDC.SelectObject(fontS);
	memDC.SetTextColor(green);
	nStep = (rect.Width() - MARGIN * 2) / L_GRID;
	steps = (theApp.status.price - theApp.settings.bid_cap_price + PRICE_INTERVAL) / MIN_ADD_INTERVAL;
	steps = (steps > 0) ? steps : 0;
	rect = CRect(MARGIN - 2, Y_GRAPH, DISP_WIDTH - MARGIN, Y_GRAPH + H_GRAPH - 1);
	memDC.SelectObject(penSolid);
	memDC.MoveTo(rect.right - 1, rect.top);
	memDC.LineTo(rect.left, rect.top);
	memDC.LineTo(rect.left, rect.bottom);
	memDC.LineTo(rect.right, rect.bottom);
	rect = CRect(MARGIN, Y_GRAPH + 2, MARGIN + L_GRID - 1, Y_GRAPH + H_GRAPH - 2);
	for(int i = 0; i < nStep; i++) {
		if(i + 3 < steps){
			memDC.FillSolidRect(&rect, red);
		} else if(i < steps) {
			memDC.FillSolidRect(&rect, green);
		} else {
			memDC.FillSolidRect(&rect, gray);
		}
		rect.OffsetRect(L_GRID, 0);
	}
	memDC.SelectObject(brush);
	{ // 警示价
		pt = CPoint(MARGIN - 1, Y_GRAPH - 5);
		FillTriangle(&memDC, pt, TRUE);
		str.Format(_T("%d"), theApp.settings.bid_cap_price);
		memDC.TextOut(pt.x + 15, pt.y - 15, str);
	}
	{ // 当前价
		pt = CPoint(MARGIN + steps * L_GRID - 1, Y_GRAPH + H_GRAPH + 4);
		FillTriangle(&memDC, pt, FALSE);
		str.Format(_T("%d"), theApp.status.price + PRICE_INTERVAL);
		memDC.TextOut(pt.x + 15, pt.y - 5, str);
	}
	if(theApp.status.autoBidStep != Status::NORMAL) { // 伏击价
		pt = CPoint(MARGIN + (theApp.status.bid_price - theApp.settings.bid_cap_price) / MIN_ADD_INTERVAL * L_GRID - 1, Y_GRAPH - 5);
		FillTriangle(&memDC, pt, TRUE);
		str.Format(_T("%d"), theApp.status.bid_price);
		memDC.TextOut(pt.x + 15, pt.y - 15, str);
	}

	//
	// YZM
	//
	rect = CRect(CPoint(0, Y_YZM), CSize(DISP_WIDTH, DISP_HEIGHT - Y_YZM));
	if(theApp.status.autoBidStep == Status::NORMAL) {
		memDC.FillSolidRect(rect, bkgnd);
	} else if(theApp.status.autoBidStep == Status::YZM) {
		rect = theApp.settings.rgn_yzm_info;
		rect.OffsetRect(theApp.settings.pt_index);
		memDC.BitBlt(MARGIN, Y_YZM + 20, rect.Width(), rect.Height(), 
			pWndIE->GetDC(), rect.left, rect.top, SRCCOPY);
		rect = theApp.settings.rgn_yzm_picture;
		rect.OffsetRect(theApp.settings.pt_index);
		memDC.StretchBlt(MARGIN, Y_YZM + 60, rect.Width()*3, rect.Height()*3, 
			pWndIE->GetDC(), rect.left, rect.top, rect.Width(), rect.Height(), SRCCOPY);
	} else {
		if(theApp.status.autoBidStep == Status::AUTO_CONFIRM) {
			memDC.FillSolidRect(&rect, green);
		} else if(theApp.status.autoBidStep == Status::CONFIRMED) {
			memDC.FillSolidRect(&rect, red);
		} else if(theApp.status.autoBidStep == Status::FINISHED) {
			memDC.FillSolidRect(&rect, violet);
		} else {
			memDC.FillSolidRect(&rect, gray); // 异常！
		}
		memDC.BitBlt(MARGIN, Y_YZM + 20, theApp.settings.rgn_yzm_info.Width(), theApp.settings.rgn_yzm_info.Height(), &dc, MARGIN, Y_YZM + 20, SRCCOPY);
		memDC.BitBlt(MARGIN, Y_YZM + 60, theApp.settings.rgn_yzm_picture.Width() * 3, theApp.settings.rgn_yzm_picture.Height() * 3, &dc, MARGIN, Y_YZM + 60, SRCCOPY);
	}

	//
	// Copy and Delete DC
	//
	dc.BitBlt(0, 0, DISP_WIDTH, DISP_HEIGHT, &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
	memBmp.DeleteObject();
}


HCURSOR CMainDlg::OnQueryDragIcon() {

	return static_cast<HCURSOR>(m_hIcon);
}


void CMainDlg::OnBnClickedHotkeyMode() {
	
	if(m_EnableHotKey.GetCheck()) {
		UnregisteHotKey();
	} else {
		RegistHotKey();
	}
}


void CMainDlg::OnBnClickedHotkeyCalc() {

	CString str;
	DWORD val = m_HotKey.GetHotKey();
	val  = (val & 0xff) | (val & 0xff00) << 8;
	str.Format(_T("热键值：0x%x (%d)"), val, val);
	AfxMessageBox(str);
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
	if(key = theApp.settings.hotkey_auto_confirm) {
		flag &= RegisterHotKey(hWnd, HOTKEY_AUTO_CONFIRM, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_escape) {
		flag &= RegisterHotKey(hWnd, HOTKEY_ESCAPE, HIWORD(key), LOWORD(key));
	}
	if(key = theApp.settings.hotkey_test_yzm) {
		flag &= RegisterHotKey(hWnd, HOTKEY_TEST_YZM, HIWORD(key), LOWORD(key));
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
	if(theApp.settings.hotkey_auto_confirm) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_AUTO_CONFIRM);
	}
	if(theApp.settings.hotkey_escape) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_ESCAPE);
	}
	if(theApp.settings.hotkey_test_yzm) {
		flag &= UnregisterHotKey(hWnd, HOTKEY_TEST_YZM);
	}

	return flag;
}


//==================================================================================================

CWnd *CMainDlg::pWndIE = NULL;
volatile BOOL CMainDlg::canNormal = FALSE;
volatile BOOL CMainDlg::isNormal = FALSE;
volatile BOOL CMainDlg::canAutoConfirm = FALSE;
volatile BOOL CMainDlg::isAutoConfirm = FALSE;


void CMainDlg::Thread_Normal(void *param) {

	CDC dc, *pDcIE = pWndIE->GetDC();
	CBitmap bitmap;
	CRect rect;
	BYTE mask[512];
	SYSTEMTIME time;
	int ocr;


	isNormal = TRUE;
	while(canNormal) {
		//
		// Init
		//
		dc.CreateCompatibleDC(pDcIE);
		//
		// Time
		//
		rect = theApp.settings.rgn_ocr_time;
		rect.OffsetRect(theApp.settings.pt_index);
		bitmap.CreateCompatibleBitmap(pDcIE, rect.Width(), rect.Height()); // 用&dc不行！
		dc.SelectObject(&bitmap);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), pDcIE, rect.left, rect.top, SRCCOPY);
		// Tools::SaveBitmap(&dc, "./ss_time.bmp");
		Tools::GetBitmapMask(mask, &dc);
		bitmap.DeleteObject();
		ocr = Tools::OCR_Number(mask, rect.Width());
		if(ocr != 0 && theApp.status.serverSecond != ocr % 100) {
			theApp.status.serverHour = (ocr / 10000) % 100;
			theApp.status.serverMinute = (ocr / 100) % 100;
			theApp.status.serverSecond = ocr % 100;
			GetLocalTime(&time);
			theApp.status.serverDelay = ((time.wSecond - theApp.status.serverSecond + 60) % 60) * 1000 + time.wMilliseconds;
		}
		//
		// Price
		//
		rect = theApp.settings.rgn_ocr_price;
		rect.OffsetRect(theApp.settings.pt_index);
		bitmap.CreateCompatibleBitmap(pDcIE, rect.Width(), rect.Height());
		dc.SelectObject(&bitmap);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), pDcIE, rect.left, rect.top, SRCCOPY);
		// Tools::SaveBitmap(&dc, "./ss_price.bmp");
		Tools::GetBitmapMask(mask, &dc);
		bitmap.DeleteObject();
		ocr = Tools::OCR_Number(mask, rect.Width());
		if(ocr != 0) {
			theApp.status.price = ocr;
		}
		//
		// Cleanup
		//
		dc.DeleteDC();
		theApp.GetMainWnd()->Invalidate(FALSE);
		Sleep(50);
	}
	isNormal = FALSE;
}


void CMainDlg::Thread_AutoConfirm(void *param) {

	SYSTEMTIME time;
	int cur_time;
	int latest_time = theApp.settings.bid.commit_before;
	int cmt_price = theApp.status.bid_price - (theApp.settings.bid.commit_advance + PRICE_INTERVAL);
	int cmt_delay = theApp.settings.bid.commit_delay;


	isAutoConfirm = TRUE;

	while(canAutoConfirm) {
		if(theApp.settings.isRealMode) {
			GetSystemTime(&time);
			cur_time = Tools::MakeTime(time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		} else {
			cur_time = Tools::MakeTime(theApp.status.serverHour, theApp.status.serverMinute, theApp.status.serverSecond);
		}
		// 到达最晚出价时间
		if(cur_time >= latest_time) {
			break;
		}
		// 达到伏击价格
		if(theApp.status.price >= cmt_price) {
			if(cmt_delay > 0) {
				Sleep((cur_time + cmt_delay < latest_time) ? cmt_delay : latest_time - cur_time);
			}
			break;
		}
		Sleep(10);
	};
	theApp.GetMainWnd()->SendMessage(WM_HOTKEY, HOTKEY_CONFIRM, theApp.settings.hotkey_confirm);
	theApp.status.autoBidStep = Status::CONFIRMED;

	if(canAutoConfirm) {
		Sleep(theApp.settings.bid_time - latest_time);
		theApp.status.autoBidStep = Status::FINISHED;
	}

	canAutoConfirm = FALSE;
	isAutoConfirm = FALSE;
}

//==================================================================================================
