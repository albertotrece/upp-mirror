#include "Draw.h"

#define LLOG(x) // RLOG(x)

AttrText::operator Value() const
{
	return RawToValue(*this);
}

void AttrText::Init()
{
	ink = Null;
	paper = Null;
	font = Null;
}

AttrText::AttrText(const char *_text)
{
	text = _text;
	Init();
}

AttrText::AttrText(const wchar *_text)
{
	text = _text;
	Init();
}

AttrText::AttrText(const WString& _text)
{
	text = _text;
	Init();
}

void Display::PaintBackground(Draw& w, const Rect& r, const Value& q,
                              Color ink, Color paper, dword style) const
{
	if(IsType<AttrText>(q)) {
		const AttrText& t = ValueTo<AttrText>(q);
		if(!IsNull(t.paper))
			paper = t.paper;
	}
	w.DrawRect(r, paper);
}

void Display::Paint(Draw& w, const Rect& r, const Value& q,
                    Color ink, Color paper, dword s) const {
	LLOG("Display::Paint: " << q << " ink:" << ink << " paper:" << paper);
	WString txt;
	Font font = StdFont();
	if(IsType<AttrText>(q)) {
		const AttrText& t = ValueTo<AttrText>(q);
		txt = t.text;
		if(!IsNull(t.paper))
			paper = t.paper;
		if(!IsNull(t.ink))
			ink = t.ink;
		if(!IsNull(t.font))
			font = t.font;
	}
	else
		txt = IsString(q) ? q : StdConvert().Format(q);
	PaintBackground(w, r, q, ink, paper, s);
	int tcy = GetTLTextHeight(w, txt, font);
	DrawTLText(w, r.left, r.top + max((r.Height() - tcy) / 2, 0), r.Width(), txt, font, ink);
}

Size Display::GetStdSize(const Value& q) const
{
	return GetTLTextSize(ScreenInfo(), WString(IsString(q) ? q : StdConvert().Format(q)), StdFont());
}

Size Display::RatioSize(const Value& q, int cx, int cy) const {
	return Size(cx, cy);
}

#ifdef flagSO
Display::Display() {}
#endif

Display::~Display() {}

GLOBAL_VAR(const Display, StdDisplay)

#ifdef flagSO
ColorDisplayNull::ColorDisplayNull(String nulltext) : nulltext(nulltext) {}
ColorDisplayNull::~ColorDisplayNull() {}
#endif

void  ColorDisplayNull::Paint(Draw& w, const Rect& r, const Value& q,
							 Color ink, Color paper, dword style) const
{
	if(IsNull(q))
		Display::Paint(w, r, nulltext, ink, paper, style);
	else
		w.DrawRect(r, Color(q));
}

const Display& ColorDisplay() { return Single<ColorDisplayNull>(); }

class SizeTextDisplayCls : public Display {
public:
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
		               Color ink, Color paper, dword style) const;
};

void  SizeTextDisplayCls::Paint(Draw& w, const Rect& r, const Value& q,
								Color ink, Color, dword) const {
	w.DrawText(r.left, r.top, r.Width(), (String)q, Arial(-r.Height()), ink);
}

const Display& SizeTextDisplay() { return Single<SizeTextDisplayCls>(); }

static inline int NScale(int sz, int r) {
	return sz ? sz < r ? r / sz * sz : r : 0;
}

class CenteredImageDisplayCls : public Display {
public:
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
		               Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, paper);
		Image m = q;
		Size sz = m.GetSize();
		if(!IsNull(m))
			w.DrawImage(r.left + (r.Width() - sz.cx) / 2, r.top + (r.Height() - sz.cy) / 2, m);
	}
	virtual Size GetStdSize(const Value& q) const
	{
		return Image(q).GetSize();
	}
};

const Display& CenteredImageDisplay() { return Single<CenteredImageDisplayCls>(); }

class CenteredHighlightImageDisplayCls : public Display {
public:
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
		               Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, paper);
		Image m = q;
		Size sz = m.GetSize();
		if(!IsNull(m))
			DrawHighlightImage(w, r.left + (r.Width() - sz.cx) / 2,
			                      r.top + (r.Height() - sz.cy) / 2, m);
	}
	virtual Size GetStdSize(const Value& q) const
	{
		return Image(q).GetSize();
	}
};

const Display& CenteredHighlightImageDisplay()
{
	return Single<CenteredHighlightImageDisplayCls>();
}

class ImageDisplayCls : public Display {
public:
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
		               Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, paper);
		Image m = q;
		if(!IsNull(m))
			w.DrawImage(r.left, r.top, Rescale(m, r.GetSize()));
	}
	virtual Size GetStdSize(const Value& q) const
	{
		return Image(q).GetSize();
	}
};

const Display& ImageDisplay() { return Single<ImageDisplayCls>(); }

class FittedImageDisplayCls : public Display {
public:
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
		               Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, paper);
		Image m = q;
		if(!IsNull(m)) {
			Size sz = GetFitSize(m.GetSize(), r.Size());
			Point p = r.CenterPos(sz);
			w.DrawImage(p.x, p.y, m);
		}
	}
	virtual Size GetStdSize(const Value& q) const
	{
		return Image(q).GetSize();
	}
};

const Display& FittedImageDisplay() { return Single<FittedImageDisplayCls>(); }

class DrawingDisplayCls : public Display {
public:
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
		               Color ink, Color paper, dword style) const;
	virtual Size GetStdSize(const Value& q) const;
	virtual Size RatioSize(const Value& q, int cx, int cy) const;
};

void DrawingDisplayCls::Paint(Draw& w, const Rect& r, const Value& q,
							  Color, Color, dword) const {
	w.DrawDrawing(r, q);
}

Size DrawingDisplayCls::GetStdSize(const Value& q) const {
	return ((const Drawing&) q).GetSize();
}

Size DrawingDisplayCls::RatioSize(const Value& q, int cx, int cy) const {
	return ((const Drawing&) q).RatioSize(cx, cy);
}

const Display& DrawingDisplay() { return Single<DrawingDisplayCls>(); }

Size  PaintRect::GetStdSize() const {
	return display ? display->GetStdSize(value) : Size(0, 0);
}

Size  PaintRect::RatioSize(int cx, int cy) const {
	return display ? display->RatioSize(value, cx, cy) : Size(0, 0);
}

void  PaintRect::Paint(Draw& w, const Rect& r,
					   Color ink, Color paper, dword style) const {
	if(display)
		display->Paint(w, r, value, ink, paper, style);
}

void  PaintRect::Paint(Draw& w, int x, int y, int cx, int cy,
					   Color ink, Color paper, dword style) const {
	Paint(w, RectC(x, y, cx, cy), ink, paper, style);
}

PaintRect::PaintRect() {
	display = NULL;
}

PaintRect::PaintRect(const Display& _display) {
	display = &_display;
}

PaintRect::PaintRect(const Display& _display, const Value& _val) {
	display = &_display;
	value = _val;
}
