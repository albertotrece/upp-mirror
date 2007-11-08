#include "IconDes.h"

NAMESPACE_UPP

Color s_stdcolor[] = {
	Black,
	Red,
	Green,
	Brown,
	Blue,
	Magenta,
	Cyan,
	Gray,
	LtGray,
	LtRed,
	LtGreen,
	LtYellow,
	LtBlue,
	LtMagenta,
	LtCyan,
	Yellow,
	WhiteGray,
	White,
};

Color RGBACtrl::GetColor(int i) const
{
	if(i < 18)
		return s_stdcolor[i];
	i -= 18;
	if(i < 18)
		return GrayColor(255 * (i + 1) / 20);
	i -= 18;
	int q = i % 18;
	i /= 18;
	Color c = Color(255 * (q < 9 ? q + 1 : 18 - q) / 9,
	                255 * (i < 6 ? i + 1 : 12 - i) / 6,
	                q < 9 ? i < 6 ? 0 : 200 : i < 6 ? 150 : 255);
	return c == White() || c == Yellow() ? Color(c.GetR(), c.GetG() - 1, c.GetB()) : c;
}

void RGBACtrl::Layout()
{
	Size sz = GetSize();
	int ah = IconDesImg::Alpha().GetHeight() + 20;
	int th = text.GetStdHeight(text.GetFont());
	int eh = 4 - 20 - ah - th - 4;
	int rh = min(sz.cx - (sz.cx >> 3), sz.cy - eh);
	ramp.BottomPos(0, rh).HSizePos();
	int y = sz.cy - rh - 4 - ah;
	alpha.TopPos(y, ah).HSizePos(1, 0);
	text.TopPos((y -= 4 + th), th).HSizePos(1, 0);
	cbox.cx = (sz.cx - 3) / 18;
	cbox.cy = minmax(cbox.cx, 4, 16);
	int ch = 14 * cbox.cy;
	cs.x = (sz.cx - 18 * cbox.cx) / 2 + 1;
	cs.y = (y -= ch + 4);
	if(subctrl)
		subctrl->TopPos(0, y - 4).HSizePos(1, 0);
}

void RGBACtrl::Paint(Draw& w)
{
	w.DrawRect(GetSize(), SColorFace);
	if(alpha.IsMask())
		return;
	for(int x = 0; x <= 18; x++)
		w.DrawRect(x * cbox.cx + cs.x, cs.y, 1, cbox.cy * 14, SColorText());
	int i = 0;
	int my = cs.y + 1;
	w.DrawRect(cs.x, cs.y + 14 * cbox.cy, cbox.cx * 18 + 1, 1, SColorText());
	Point pp = Null;
	for(int y = 0; y < 14; y++) {
		w.DrawRect(cs.x, my - 1, cbox.cx * 18 + 1, 1, SColorText());
		int mx = cs.x + 1;
		for(int x = 0; x < 18; x++) {
			Color c = GetColor(i++);
			w.DrawRect(mx, my, cbox.cx - 1, cbox.cy - 1, c);
			if(c == color)
				pp = Point(mx, my);
			mx += cbox.cx;
		}
		my += cbox.cy;
	}
	if(!IsNull(pp)) {
		Size isz = CtrlImg::wheel_cursor().GetSize();
		pp = pp + (cbox - isz) / 2;
		w.DrawImage(pp.x, pp.y, CtrlImg::wheel_cursor(),
		            Grayscale(color) < 120 ? White() : Black());
	}
}

void RGBACtrl::LeftDown(Point p, dword keyflags)
{
	int x = (p.x - cs.x) / cbox.cx;
	int y = (p.y - cs.y) / cbox.cy;
	if(x >= 0 && x < 18 && y >= 0 && y < 18) {
		color = GetColor(x + 18 * y);
		ramp <<= color;
		alpha.Set(255);
		alpha.SetColor(color);
		UpdateActionRefresh();
		SyncText();
	}
}

void RGBACtrl::MouseMove(Point p, dword keyflags)
{
	if(keyflags & K_MOUSELEFT)
		LeftDown(p, keyflags);
}

void RGBACtrl::SyncText()
{
	RGBA x = Get();
	text <<= Format("RGBA(%3d, %3d, %3d, %3d)", (int)x.r, (int)x.g, (int)x.b, (int)x.a);
}

void RGBACtrl::Ramp()
{
	color = ~ramp;
	alpha.SetColor(color);
	UpdateActionRefresh();
	SyncText();
}

void RGBACtrl::Alpha()
{
	SyncText();
	UpdateAction();
}


void RGBACtrl::MaskSet(int a)
{
	alpha.Set(a);
	UpdateRefresh();
	SyncText();
}

void RGBACtrl::Set(RGBA c)
{
	Unmultiply(&c, &c, 1);
	color = StraightColor(c);
	ramp <<= color;
	alpha.SetColor(color);
	MaskSet(c.a);
}

RGBA RGBACtrl::Get() const
{
	RGBA c = color;
	c.a = alpha.Get();
	if(alpha.IsMask())
		c.r = c.g = c.b = c.a;
	Premultiply(&c, &c, 1);
	return c;
}

void RGBACtrl::Mask(bool b)
{
	alpha.Mask(b);
	ramp.Show(!b);
	Refresh();
}

void RGBACtrl::SubCtrl(Ctrl *c)
{
	if(subctrl)
		subctrl->Remove();
	subctrl = c;
	if(c)
		Add(*c);
	Layout();
}

RGBACtrl::RGBACtrl()
{
	color = Black();
	BackPaint();
	Add(ramp);
	ramp <<= THISBACK(Ramp);
	Add(alpha);
	alpha <<= THISBACK(Alpha);
	text.SetFont(Courier(11));
	text.NoWantFocus();
	text.SetReadOnly();
	Add(text);
	SyncText();
}

END_UPP_NAMESPACE
