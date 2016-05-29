#pragma once


// CMyHotKey

class CMyHotKey : public CHotKeyCtrl
{
	DECLARE_DYNAMIC(CMyHotKey)

public:
	CMyHotKey();
	virtual ~CMyHotKey();

protected:
	DECLARE_MESSAGE_MAP()
};
