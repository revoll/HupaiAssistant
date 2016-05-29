#pragma once


class Tools
{
public:
	Tools(void);
	~Tools(void);

public:
	static int MakeTime(int h, int m, int s, int ms=0);
	static int TimeFromStr(const char *);
	static CString TimeToStr(int);

	static CPoint PointFromStr(const char *);
	static CString PointToStr(const CPoint *);

	static CRect RectFromStr(const char *);
	static CString RectToStr(const CRect *);

	static LONG LongFromStr(const char *);

	static BOOL SaveBitmap(const CDC *, const char *);
	static BOOL GetBitmapMask(void *, const CDC *);

	static int OCR_Number(const void *, int w, int h=10); // FLASE:-1, TRUE:>=0
};
