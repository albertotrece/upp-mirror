#include "CtrlCore.h"

NAMESPACE_UPP

#define LLOG(x) //  LOG(x)

Ptr<Ctrl> Ctrl::eventCtrl;
Ptr<Ctrl> Ctrl::mouseCtrl;
Ptr<Ctrl> Ctrl::captureCtrl;
Ptr<Ctrl> Ctrl::repeatTopCtrl;
Point     Ctrl::repeatMousePos;
bool      Ctrl::ignoreclick;
bool      Ctrl::mouseinframe;
bool      Ctrl::mouseinview;
Point     Ctrl::mousepos;
Point     Ctrl::leftmousepos = Null;
Point     Ctrl::rightmousepos = Null;;

dword GetMouseFlags() {
	dword style = 0;
	if(GetAlt()) style |= K_ALT;
	if(GetCtrl()) style |= K_CTRL;
	if(GetShift()) style |= K_SHIFT;
	if(GetMouseLeft()) style |= K_MOUSELEFT;
	if(GetMouseRight()) style |= K_MOUSERIGHT;
	if(GetMouseMiddle()) style |= K_MOUSELEFT|K_CTRL|K_MOUSEMIDDLE;
	return style;
}

void Ctrl::LogMouseEvent(const char *f, const Ctrl *ctrl, int event, Point p, int zdelta, dword keyflags)
{
	if(!IsUsrLog())
		return;
	String txt = f;
	txt = (event & BUTTON) == RIGHT ? "RIGHT" : "LEFT";
	switch(event & ACTION) {
	case DOWN:        txt << "DOWN"; break;
	case UP:          txt << "UP"; break;
	case DOUBLE:      txt << "DOUBLE"; break;
	case MOUSEWHEEL:  txt << "WHEEL"; break;
	default:
		return;
	}
	txt << ' ' << p << " (";
	if(keyflags & K_ALT)
		txt << " ALT";
	if(keyflags & K_CTRL)
		txt << " CTRL";
	if(keyflags & K_SHIFT)
		txt << " SHIFT";
	if(keyflags & K_MOUSELEFT)
		txt << " LEFT";
	if(keyflags & K_MOUSERIGHT)
		txt << " RIGHT";
	txt << " ) " << Desc(ctrl);
	UsrLogT(txt);
}

Image Ctrl::FrameMouseEventH(int event, Point p, int zdelta, dword keyflags)
{
	for(int i = 0; i < mousehook().GetCount(); i++)
		if((*mousehook()[i])(this, true, event, p, zdelta, keyflags))
			return Image::Arrow();
	LogMouseEvent("FRAME ", this, event, p, zdelta, keyflags);
	eventCtrl = this;
	if(parent)
		parent->ChildFrameMouseEvent(this, event, p, zdelta, keyflags);
	return FrameMouseEvent(event, p, zdelta, keyflags);
}

Image Ctrl::FrameMouseEvent(int event, Point p, int zdelta, dword keyflags)
{
	return Image::Arrow();
}

Image Ctrl::MouseEventH(int event, Point p, int zdelta, dword keyflags)
{
	for(int i = 0; i < mousehook().GetCount(); i++)
		if((*mousehook()[i])(this, false, event, p, zdelta, keyflags))
			return Image::Arrow();
	LogMouseEvent(NULL, this, event, p, zdelta, keyflags);
	if(parent)
		parent->ChildMouseEvent(this, event, p, zdelta, keyflags);
	return MouseEvent(event, p, zdelta, keyflags);
}

void Ctrl::ChildFrameMouseEvent(Ctrl *child, int event, Point p, int zdelta, dword keyflags)
{
	if(parent)
		parent->ChildFrameMouseEvent(child, event, p, zdelta, keyflags);
}

void Ctrl::ChildMouseEvent(Ctrl *child, int event, Point p, int zdelta, dword keyflags)
{
	if(parent)
		parent->ChildMouseEvent(child, event, p, zdelta, keyflags);
}

Image Ctrl::MouseEvent(int event, Point p, int zdelta, dword keyflags)
{
	switch(event) {
	case MOUSEENTER:
		MouseEnter(p, keyflags);
		break;
	case MOUSEMOVE:
		MouseMove(p, keyflags);
		break;
	case LEFTDOWN:
		LeftDown(p, keyflags);
		break;
	case LEFTDOUBLE:
		LeftDouble(p, keyflags);
		break;
	case LEFTDRAG:
		LeftDrag(p, keyflags);
		break;
	case LEFTHOLD:
		LeftHold(p, keyflags);
		break;
	case LEFTTRIPLE:
		LeftTriple(p, keyflags);
		break;
	case RIGHTDRAG:
		RightDrag(p, keyflags);
		break;
	case RIGHTHOLD:
		RightHold(p, keyflags);
		break;
	case RIGHTTRIPLE:
		RightTriple(p, keyflags);
		break;
	case RIGHTDOWN:
		RightDown(p, keyflags);
		break;
	case RIGHTDOUBLE:
		RightDouble(p, keyflags);
		break;
	case LEFTREPEAT:
		LeftRepeat(p, keyflags);
		break;
	case RIGHTREPEAT:
		RightRepeat(p, keyflags);
		break;
	case LEFTUP:
		LeftUp(p, keyflags);
		break;
	case RIGHTUP:
		RightUp(p, keyflags);
		break;
	case MOUSELEAVE:
		MouseLeave();
		break;
	case MOUSEWHEEL:
		MouseWheel(p, zdelta, keyflags);
		break;
	case CURSORIMAGE:
		return CursorImage(p, keyflags);
	}
	return Image::Arrow();
}

Image Ctrl::CursorImage(Point p, dword keyflags)
{
	return Image::Arrow();
}

void Ctrl::IgnoreMouseClick()
{
	LLOG("IgnoreMouseClick");
	ignoreclick = true;
	KillRepeat();
}

void Ctrl::IgnoreMouseUp()
{
	LLOG("Ctrl::IgnoreMouseUp");
	if(GetMouseLeft() || GetMouseRight() || GetMouseMiddle())
		IgnoreMouseClick();
}

void Ctrl::EndIgnore()
{
	LLOG("Ctrl::EndIgnore");
	if(GetMouseLeft() || GetMouseRight() || GetMouseMiddle()) return;
	KillRepeat();
	ignoreclick = false;
}

bool Ctrl::IsMouseActive() const
{
	return IsVisible() && IsEnabled() && IsOpen() && !ignoremouse;
}

Ctrl *Ctrl::ChildFromPoint(Point& pt) const
{
	Ctrl *q;
	Point p = pt;
	Rect rect = GetRect();
	Rect view = GetView();
	for(q = GetLastChild(); q; q = q->prev) {
		if(q->InFrame() && q->IsMouseActive()) {
			Rect r = q->GetRect();
			if(r.Contains(p)) {
				pt = p - r.TopLeft();
				return q;
			}
		}
	}
	if(view.Contains(p)) {
		p -= view.TopLeft();
		for(q = GetLastChild(); q; q = q->prev) {
			if(q->InView() && q->IsMouseActive()) {
				Rect r = q->GetRect();
				if(r.Contains(p)) {
					pt = p - r.TopLeft();
					return q;
				}
			}
		}
	}
	return NULL;
}

Image Ctrl::MEvent0(int e, Point p, int zd)
{
	LLOG("MEvent0 " << Name() << " event: " << e << " point:" << p);
	Ptr<Ctrl> _this = this;
	mousepos = p;
	dword mm = 0;
	if(zd == MIDDLEBUTTON) {
		mm = K_MOUSEMIDDLE;
		zd = 0;
	}
	if((e & ACTION) == DOUBLE)
		mm |= K_MOUSEDOUBLE;
	if((e & ACTION) == TRIPLE)
		mm |= K_MOUSETRIPLE;
	Rect view = GetView();
	if(mouseCtrl != this) {
		if(mouseCtrl) {
			Ptr<Ctrl> mousectrl = mouseCtrl;
			mousectrl->MouseEventH(MOUSELEAVE, Point(0, 0), zd, GetMouseFlags() | mm);
			if(mousectrl)
				mousectrl->FrameMouseEventH(MOUSELEAVE, Point(0, 0), zd, GetMouseFlags() | mm);
		}
		mouseinframe = mouseinview = false;
		if(_this) {
			mouseCtrl = _this;
			mouseinframe = true;
			FrameMouseEventH(MOUSEENTER, p, zd, GetMouseFlags() | mm);
		}
	}
	bool inview = view.Contains(p);
	if(inview != mouseinview && _this) {
		mouseinview = inview;
		MouseEventH(inview ? MOUSEENTER : MOUSELEAVE, p, zd, GetMouseFlags() | mm);
	}
	if(_this)
		if(view.Contains(p) || HasCapture()) {
			p -= view.TopLeft();
			return MouseEventH(e, p, zd, GetMouseFlags() | mm);
		}
		else
			return FrameMouseEventH(e, p, zd, GetMouseFlags() | mm);
	return Image::Arrow();
}

void    Ctrl::LRepeat() {
	if(repeatTopCtrl && repeatTopCtrl->HasFocusDeep()) // 4.7.2004 cxl, IsForeground...
		repeatTopCtrl->DispatchMouseEvent(LEFTREPEAT, repeatMousePos, 0);
	else
		KillRepeat();
	LLOG("LRepeat " << UPP::Name(mouseCtrl));
}

static int sDistMax(Point a, Point b)
{
	return IsNull(a) ? INT_MAX : max(abs(a.x - b.x), abs(a.y - b.y));
}

static int sDistMin(Point a, Point b)
{
	return IsNull(a) ? -1 : max(abs(a.x - b.x), abs(a.y - b.y));
}

void    Ctrl::LRep() {
	LLOG("LRep");
	UPP::SetTimeCallback(-GetKbdSpeed(), callback(&Ctrl::LRepeat), &mousepos);
}

void    Ctrl::LHold() {
	if(sDistMax(leftmousepos, mousepos) < GUI_DragDistance() && repeatTopCtrl)
		repeatTopCtrl->DispatchMouseEvent(LEFTHOLD, repeatMousePos, 0);
}

void    Ctrl::RRepeat() {
	if(repeatTopCtrl && repeatTopCtrl->IsForeground())
		repeatTopCtrl->DispatchMouseEvent(RIGHTREPEAT, repeatMousePos, 0);
	else
		KillRepeat();
}

void    Ctrl::RRep() {
	UPP::SetTimeCallback(-GetKbdSpeed(), callback(&Ctrl::RRepeat), &mousepos);
}

void    Ctrl::RHold() {
	if(sDistMax(rightmousepos, mousepos) < GUI_DragDistance() && repeatTopCtrl)
		repeatTopCtrl->DispatchMouseEvent(RIGHTHOLD, repeatMousePos, 0);
}

void    Ctrl::KillRepeat() {
	LLOG("Ctrl::KillRepeat");
	UPP::KillTimeCallback(&mousepos);
}

bool    Ctrl::HasMouse() const
{
	return mouseCtrl == this;
}

bool    Ctrl::HasMouseDeep() const
{
	return mouseCtrl == this || HasChildDeep(mouseCtrl);
}

Ctrl   *Ctrl::GetMouseCtrl()
{
	return mouseCtrl;
}

bool    Ctrl::HasMouseInFrame(const Rect& r)
{
	if(!HasMouse())
		return false;
	Rect q = GetVisibleScreenRect();
	q = r.Offseted(q.TopLeft()) & q;
	return q.Contains(GetMousePos());
}

bool    Ctrl::HasMouseIn(const Rect& r) const
{
	if(!HasMouse())
		return false;
	Rect q = GetVisibleScreenView();
	q = r.Offseted(q.TopLeft()) & q;
	return q.Contains(GetMousePos());
}

Point Ctrl::GetMouseViewPos() const
{
	return GetMousePos() - GetVisibleScreenView().TopLeft();
}

void    Ctrl::DoCursorShape() {
//	LLOG("DoCursorShape " << ::Name(mouseCtrl));
	if(mouseCtrl) {
		Image m = CursorOverride();
		if(IsNull(m))
			m = mouseCtrl->MEvent0(CURSORIMAGE, mousepos, 0);
		mouseCtrl->GetTopCtrl()->SetMouseCursor(m);
	}
}

void    Ctrl::CheckMouseCtrl() {
	Point p = GetMousePos();
	if(mouseCtrl) {
		Rect r = mouseCtrl->GetScreenRect();
		LLOG("CheckMouseCtrl mouseCtrl " << UPP::Name(mouseCtrl) << " " << r);
		if(!mouseCtrl->HasCapture() && !r.Contains(p)) {
			Ptr<Ctrl> mousectrl = mouseCtrl;
			if(mouseinview)
				mousectrl->MouseEventH(MOUSELEAVE, p - mousectrl->GetScreenView().TopLeft(),
				                       0, GetMouseFlags());
			if(mouseinframe && mousectrl)
				mousectrl->FrameMouseEventH(MOUSELEAVE, p - r.TopLeft(),
				                            0, GetMouseFlags());
			mouseinview = mouseinframe = false;
			mouseCtrl = NULL;
			leftmousepos = rightmousepos = Null;
		}
	}
	DoCursorShape();
}

Point leftdblpos = Null, rightdblpos = Null;
int leftdbltime = Null, rightdbltime = Null;

bool sDblTime(int time)
{
	return !IsNull(time) && (int)GetTickCount() - time < GUI_DblClickTime();
}

Image Ctrl::DispatchMouse(int e, Point p, int zd) {
	if(e == MOUSEMOVE) {
		if(sDistMin(leftmousepos, mousepos) > GUI_DragDistance() && repeatTopCtrl == this) {
			DispatchMouseEvent(LEFTDRAG, leftmousepos, 0);
			leftmousepos = Null;
		}
		if(sDistMin(rightmousepos, mousepos) > GUI_DragDistance() && repeatTopCtrl == this) {
			DispatchMouseEvent(RIGHTDRAG, rightmousepos, 0);
			rightmousepos = Null;
		}
	}
	repeatMousePos = p;
	repeatTopCtrl = this;
	if(e == LEFTDOUBLE) {
		leftdbltime = GetTickCount();
		leftdblpos = p;
	}
	if(e == RIGHTDOUBLE) {
		rightdbltime = GetTickCount();
		rightdblpos = p;
	}
	if(e == LEFTDOWN) {
		LLOG("Ctrl::DispatchMouse: init left repeat for " << UPP::Name(this) << " at " << p);
		UPP::SetTimeCallback(GetKbdDelay(), callback(&Ctrl::LRep), &mousepos);
		UPP::SetTimeCallback(2 * GetKbdDelay(), callback(&Ctrl::LHold), &mousepos);
		leftmousepos = p;
		if(sDistMax(leftdblpos, p) < GUI_DragDistance() && sDblTime(leftdbltime))
			e = LEFTTRIPLE;
	}
	if(e == RIGHTDOWN) {
		LLOG("Ctrl::DispatchMouse: init right repeat for " << UPP::Name(this) << " at " << p);
		UPP::SetTimeCallback(GetKbdDelay(), callback(&Ctrl::RRep), &mousepos);
		UPP::SetTimeCallback(2 * GetKbdDelay(), callback(&Ctrl::RHold), &mousepos);
		rightmousepos = p;
		if(sDistMax(rightdblpos, p) < GUI_DragDistance() && sDblTime(rightdbltime))
			e = RIGHTTRIPLE;
	}
	if(e == LEFTUP)
		leftmousepos = Null;
	if(e == RIGHTUP)
		rightmousepos = Null;
	if(e == LEFTUP || e == RIGHTUP)
		KillRepeat();
	return DispatchMouseEvent(e, p, zd);
}

Image Ctrl::DispatchMouseEvent(int e, Point p, int zd) {
	if(!IsEnabled())
		return Image::Arrow();
	if(captureCtrl && captureCtrl != this && captureCtrl->IsMouseActive())
		return captureCtrl->MEvent0(e, p + GetScreenRect().TopLeft() -
		                            captureCtrl->GetScreenRect().TopLeft(), zd);
	Ctrl *top = this;
	if(e == MOUSEWHEEL && !GetParent()) {
		Ctrl *w = GetFocusCtrl();
		if(w) {
			top = w->GetTopCtrl();
			p = GetMousePos() - top->GetScreenRect().TopLeft();
		}
	}
	Ctrl *q = top->ChildFromPoint(p);
	return q ? q->DispatchMouseEvent(e, p, zd) : top->MEvent0(e, p, zd);
}

bool Ctrl::SetCapture() {
	ReleaseCtrlCapture();
	if(!GetTopCtrl()->SetWndCapture()) return false;
	captureCtrl = mouseCtrl = this;
	return true;
}

bool Ctrl::ReleaseCapture() {
	return this == captureCtrl && ReleaseCtrlCapture();
}

bool Ctrl::ReleaseCtrlCapture() {
	if(captureCtrl) {
		captureCtrl->CancelMode();
		Ctrl *w = captureCtrl->GetTopCtrl();
		captureCtrl = NULL;
		if(w->HasWndCapture()) {
			w->ReleaseWndCapture();
			return true;
		}
	}
	captureCtrl = NULL;
	return false;
}

bool Ctrl::HasCapture() const {
	return captureCtrl == this && GetTopCtrl()->HasWndCapture();
}

Ctrl *Ctrl::GetVisibleChild(Ctrl *ctrl, Point p, bool pointinframe)
{
	if(!pointinframe)
		p += ctrl->GetView().TopLeft();
	Ctrl *q;
	Rect rect = ctrl->GetRect();
	for(q = ctrl->GetLastChild(); q; q = q->GetPrev()) {
		if(q->InFrame() && q->IsVisible()) {
			Rect r = q->GetRect();
			if(r.Contains(p))
				return GetVisibleChild(q, p - r.TopLeft(), true);
		}
	}
	Rect view = ctrl->GetView();
	if(view.Contains(p)) {
		p -= view.TopLeft();
		for(q = ctrl->GetLastChild(); q; q = q->GetPrev()) {
			if(q->InView() && q->IsVisible()) {
				Rect r = q->GetRect();
				if(r.Contains(p))
					return GetVisibleChild(q, p - r.TopLeft(), true);
			}
		}
	}
	return ctrl;
}

AutoWaitCursor::AutoWaitCursor(int& avg) : WaitCursor(avg >= 0), avg(avg) {
	time0 = GetTickCount();
}

AutoWaitCursor::~AutoWaitCursor() {
	if(time0) avg = GetTickCount() - time0 - 500;
	if(avg < -10000) avg = -10000;
	if(avg >  10000) avg = 10000;
}

Image& Ctrl::CursorOverride()
{
	static Image m;
	return m;
}

Image Ctrl::OverrideCursor(const Image& m)
{
	Image om = CursorOverride();
	CursorOverride() = m;
	DoCursorShape();
	return om;
}

END_UPP_NAMESPACE
