//$ class Ctrl {
private:
	static void AddInvalid(const Rect& rect);

	static Ptr<Ctrl>      desktop;
	static Vector<Ctrl *> topctrl;
	static ImageBuffer    framebuffer;
	static Vector<Rect>   invalid;

	static Point fbCursorPos;
	static Image fbCursorImage;
	
	static Point fbCursorBakPos;
	static Image fbCursorBak;
	
	static Rect  fbCaretRect;
	static Image fbCaretBak;
	static int   fbCaretTm;

	static Image GetBak(Rect& tr);
	static void RemoveCursor();
	static void RemoveCaret();
	static void CursorSync();
	
	int FindTopCtrl() const;
	static Rect GetClipBound(const Vector<Rect>& inv, const Rect& r);

	friend struct PaintProxy__;

public:
	static void DoMouseFB(int event, Point p, int zdelta = 0);
	static bool DoKeyFB(dword key, int cnt);
	
	void  SetOpen(bool b)               { isopen = b; }
	void  SetTop()                      { top = new Top; }

	static void InitFB();

	static void  SetDesktop(Ctrl& q);
	static Ctrl *GetDesktop()                  { return desktop; }
	static void  SetFramebufferSize(Size sz);
	
	static const ImageBuffer& GetFrameBuffer() { return framebuffer; }

//$ };
