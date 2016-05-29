
// HupaiAssistant.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"					// 主符号

#define CONFIG_FILE_PATH				"./config.xml"


class Settings
{
public:
	Settings();

public:
	// 加载配置文件 & 保存配置
	BOOL LoadSettings(void);
	BOOL SaveSettings(void);

public:
//------------------------------------------------------------------------------
	//
	// 配置文件版本信息
	//
	CString cfg_version;
	CString cfg_time;
	CString cfg_desc;
	BOOL	isRealMode;

	//
	// 拍卖信息
	//
	CString bid_date;
	CString bid_time;
	CString bid_amount;
	CString bid_cap_price;

	//
	// IE浏览器设置
	//
	int ie_width;
	int ie_height;
	CString ie_url;

	//
	// 国拍系统界面布局
	//
	CPoint pt_index;					// 参考点（左框左上角的点）在客户区的位置
	CPoint pt_jiajia;					// 加价按钮
	CPoint pt_jiajia_input;				// 加价输入框
	CPoint pt_chujia;					// 出价按钮
	CPoint pt_chujia_input;				// 价格输入框
	CPoint pt_yzm_input;				// 验证码：输入
	CPoint pt_yzm_refresh;				// 验证码：刷新
	CPoint pt_yzm_ok;					// 验证码：确认
	CPoint pt_yzm_cancle;				// 验证码：取消
	CPoint pt_ok;						// 只有一个按钮的情况：确认
	CRect rgn_ocr_time;					// 文字识别：服务器时间
	CRect rgn_ocr_price;				// 文字识别：当前最低价
	CRect rgn_yzm_picture;				// 验证码图片框
	CRect rgn_yzm_info;					// 验证码提示信息框

	//
	// 系统热键
	//
	DWORD hotkey_start_fuji;				// 触发默认出价程序（伏击）
	DWORD hotkey_ok;
	DWORD hotkey_refresh;
	DWORD hotkey_clear;
	DWORD hotkey_confirm;
	DWORD hotkey_cancle;
	DWORD hoteky_enter;
	DWORD hoteky_escape;

	//
	// 自动伏击模式
	//
	struct Bid {
		Bid();
		BOOL auto_trigger;				// 自动触发：到了触发时间自动出价？
		int trigger_time;				// 出价时间：一天中的第几毫秒
		int add_price;					// 最低价加价幅度
		int commit_advance;				// 最高价提前出价量
		int commit_delay;				// 到达最高价延迟多少毫秒出价
		int commit_before;				// 最晚出价时间（强制出价）
	} bid;								// == 伏击模式：48s+700 ==
//------------------------------------------------------------------------------
};


class Status
{
public:
	Status();
	volatile int serverHour;
	volatile int serverMinute;
	volatile int serverSecond;
	volatile int serverDelay;
	volatile int price;
};


class CHupaiAssistantApp : public CWinApp
{
public:
	CHupaiAssistantApp();

public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()

public:
	Settings	settings;
	Status		status;
};

extern CHupaiAssistantApp theApp;
