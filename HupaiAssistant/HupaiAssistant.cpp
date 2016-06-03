
// HupaiAssistant.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "HupaiAssistant.h"
#include "MainDlg.h"
#include "TinyXML/tinyxml.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//--------------------------------------------------------------------------------------------------

//
// Class: Setting
//

Settings::Settings() {

	if(FALSE == LoadSettings()) {
		AfxMessageBox(_T("加载Config.xml文件时发现异常！"));
	}
}


BOOL Settings::LoadSettings() {

	TiXmlDocument config_doc(CONFIG_FILE_PATH);
	TiXmlElement *eRoot, *e, *e1;
#define TI_FETCH(E, STR)				{ E = e->FirstChildElement(STR); ASSERT(E); }


	//
	// 加载配置文件
	//
	config_doc.LoadFile(TIXML_ENCODING_UTF8);
	if((eRoot = config_doc.RootElement()) == NULL) {
		return FALSE;
	}

	//
	// 读取配置文件属性
	//
	if(this->cfg_version = eRoot->Attribute("version")) {
		ASSERT(this->cfg_time = eRoot->Attribute("time"));
		ASSERT(this->cfg_desc = eRoot->Attribute("description"));
		ASSERT(eRoot->Attribute("real_mode"));
		this->isRealMode = (strcmp(eRoot->Attribute("real_mode"), "0") == 0) ? FALSE : TRUE;
	} else {
		return FALSE;
	}

	//
	// 读取本次拍卖信息
	//
	if(e = eRoot->FirstChildElement("BidInfo")) {
		// 竞拍日期
		TI_FETCH(e1, "BidDate");
		this->bid_date = e1->GetText();
		// 竞拍结束时间
		TI_FETCH(e1, "EndTime");
		this->bid_time = Tools::TimeFromStr(e1->GetText());
		// 沪牌指标
		TI_FETCH(e1, "Amount");
		this->bid_amount = atoi(e1->GetText());
		// 警示价
		TI_FETCH(e1, "CapPrice");
		this->bid_cap_price = atoi(e1->GetText());
	} else {
		return FALSE;
	}

	//
	// 读取IE浏览器配置
	//
	if(e = eRoot->FirstChildElement("IE")) {
		// 浏览器大小
		TI_FETCH(e1, "Width");
		this->ie_width = atoi(e1->GetText());
		TI_FETCH(e1, "Height");
		this->ie_height = atoi(e1->GetText());
		// URL
		if(isRealMode) {
			TI_FETCH(e1, "URL");
		} else {
			TI_FETCH(e1, "URL_Practice");
		}
		this->ie_url = e1->GetText();
	} else {
		return FALSE;
	}

	//
	// 读取客户端布局下，各个按钮的位置坐标
	//
	if(e = isRealMode ? eRoot->FirstChildElement("Position_alltobid") : eRoot->FirstChildElement("Position_51hupai")) {
		// 参考点的坐标
		TI_FETCH(e1, "Index");
		this->pt_index = Tools::PointFromStr(e1->GetText());
		// 加价
		TI_FETCH(e1, "JJ_Input");
		this->pt_jiajia_input = Tools::PointFromStr(e1->GetText());
		TI_FETCH(e1, "JJ_Button");
		this->pt_jiajia = Tools::PointFromStr(e1->GetText());
		// 出价
		TI_FETCH(e1, "CJ_Input");
		this->pt_chujia_input = Tools::PointFromStr(e1->GetText());
		TI_FETCH(e1, "CJ_Button");
		this->pt_chujia = Tools::PointFromStr(e1->GetText());
		// 验证码部分
		TI_FETCH(e1, "YZM_Input");
		this->pt_yzm_input = Tools::PointFromStr(e1->GetText());
		TI_FETCH(e1, "YZM_Refresh");
		this->pt_yzm_refresh = Tools::PointFromStr(e1->GetText());
		TI_FETCH(e1, "YZM_Submit");
		this->pt_yzm_ok = Tools::PointFromStr(e1->GetText());
		TI_FETCH(e1, "YZM_Cancle");
		this->pt_yzm_cancle = Tools::PointFromStr(e1->GetText());
		// 确认按钮：泛指单个按钮情况
		TI_FETCH(e1, "OK_Button");
		this->pt_ok = Tools::PointFromStr(e1->GetText());
		// OCR文字识别部分（区域）
		TI_FETCH(e1, "OCR_Time");
		this->rgn_ocr_time = Tools::RectFromStr(e1->GetText());
		TI_FETCH(e1, "OCR_Price");
		this->rgn_ocr_price = Tools::RectFromStr(e1->GetText());
		TI_FETCH(e1, "YZM_Picture");
		this->rgn_yzm_picture = Tools::RectFromStr(e1->GetText());
		TI_FETCH(e1, "YZM_Info");
		this->rgn_yzm_info = Tools::RectFromStr(e1->GetText());
	} else {
		return FALSE;
	}

	//
	// 热键设置
	//
	if(e = eRoot->FirstChildElement("HotKey")) {
		// 点击确认按钮：对话框包含一个按钮情况下
		TI_FETCH(e1, "PressOK");
		this->hotkey_ok = Tools::LongFromStr(e1->GetText());
		// 点击确认验证码按钮
		TI_FETCH(e1, "PressConfirm");
		this->hotkey_confirm = Tools::LongFromStr(e1->GetText());
		// 点击取消验证码按钮
		TI_FETCH(e1, "PressCancle");
		this->hotkey_cancle = Tools::LongFromStr(e1->GetText());
		// 刷新验证码
		TI_FETCH(e1, "PressRefresh");
		this->hotkey_refresh = Tools::LongFromStr(e1->GetText());
		// 清空验证码
		TI_FETCH(e1, "ClearYZM");
		this->hotkey_clear = Tools::LongFromStr(e1->GetText());
		// 启动默认伏击策略：如48s+700。
		TI_FETCH(e1, "ChuJia");
		this->hotkey_chujia = Tools::LongFromStr(e1->GetText());
		// 退出任何自动模式
		TI_FETCH(e1, "AutoConfirm");
		this->hotkey_auto_confirm = Tools::LongFromStr(e1->GetText());
		// 退出任何自动模式
		TI_FETCH(e1, "Escape");
		this->hotkey_escape = Tools::LongFromStr(e1->GetText());
		// 测试：预览验证码
		TI_FETCH(e1, "TestYZM");
		this->hotkey_test_yzm = Tools::LongFromStr(e1->GetText());
	} else {
		return FALSE;
	}

	//
	// 读取竞拍模式 & 伏击配置
	//
	if(e = eRoot->FirstChildElement("AutoBid")) {
		// 到了指定时间自动启动 或 手动启动 （AutoTrigger = 0 或 1）
		TI_FETCH(e1, "AutoTrigger");
		bid.auto_trigger = ("1" == e1->GetText()) ? TRUE : FALSE;
		TI_FETCH(e1, "TriggerTime");
		bid.trigger_time = Tools::TimeFromStr(e1->GetText());
		// 最低价加价量
		TI_FETCH(e1, "AddPrice");
		bid.add_price = atoi(e1->GetText());
		// 提前多少出价？
		TI_FETCH(e1, "CommitAdvance");
		bid.commit_advance = atoi(e1->GetText());
		// 到达伏击价时，延迟多少时间再出价？
		TI_FETCH(e1, "CommitDelay");
		bid.commit_delay = Tools::TimeFromStr(e1->GetText());
		// 最晚出价时间：在此时间必须出价！
		TI_FETCH(e1, "CommitBefore");
		bid.commit_before = Tools::TimeFromStr(e1->GetText());
	} else {
		return FALSE;
	}

	//
	// 配置信息加载结束
	//
	return TRUE;
}


BOOL Settings::SaveSettings() {

	return FALSE;
}


Status::Status() {

	serverHour = serverMinute = serverSecond = 0;
	serverDelay = 0;
	price = 0;
	bid_price = 0;
	autoBidStep = Status::NORMAL;
}

//--------------------------------------------------------------------------------------------------


//
// CHupaiAssistantApp
//

BEGIN_MESSAGE_MAP(CHupaiAssistantApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CHupaiAssistantApp::CHupaiAssistantApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


CHupaiAssistantApp theApp;


BOOL CHupaiAssistantApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = NULL; // new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	} else if (nResponse == IDCANCEL) {
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL) {
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
