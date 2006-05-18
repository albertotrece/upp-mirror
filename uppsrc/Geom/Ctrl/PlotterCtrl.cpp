#include "GeomCtrl.h"
#pragma hdrstop

#define IMAGESPACE PlotterImg
#define IMAGEFILE  <Geom/Ctrl/PlotterCtrl.iml>
#include           <Draw/iml.h>

#define LLOG(x) // LOG(x)
#define LLOGBLOCK(x) // LOGBLOCK(x)

PlotterCtrl::ViewPlot::ViewPlot(PlotterCtrl& ctrl, int extra_gap)
: viewdraw(&ctrl)
{
	Plotter::Set(viewdraw, ctrl.scale, ctrl.delta, extra_gap);
	PathMap(&PathStyleMap::App());
}

PlotterCtrl::ViewPlot::ViewPlot(PlotterCtrl& ctrl, Pointf scale, Pointf delta, int extra_gap)
: viewdraw(&ctrl)
{
	Plotter::Set(viewdraw, scale, delta, extra_gap);
	PathMap(&PathStyleMap::App());
}

PlotterCtrl::ViewPlot::ViewPlot(PlotterCtrl& ctrl, const Matrixf& preform, int extra_gap)
: viewdraw(&ctrl)
{
	Plotter::Set(viewdraw, ctrl.scale, ctrl.delta, extra_gap);
	Plotter::Set(viewdraw, preform * physical, extra_gap);
	PathMap(&PathStyleMap::App());
}

PlotterCtrl::PlotterCtrl()
: extent(Null)
, scale(1, 1)
, delta(0, 0)
, push_scale(1, 1)
, push_delta(0, 0)
, max_scale(1e20, 1e20)
, aspect(1)
, gap(10, 10, 10, 10)
, rev_x(false)
, rev_y(false)
, halign(ALIGN_CENTER)
, valign(ALIGN_CENTER)
, old_size(Null)
//, paint_draw(NULL)
, buffer_paint(false)
, show_scroll(true)
, lock_drag_drop(false)
, lock_short_drag_drop(false)
, is_painting(false)
, background(White())
{
	hscroll  .NoAutoHide().NoAutoDisable();
	vscroll  .NoAutoHide().NoAutoDisable();
	horz_in  .SetImage(PlotterImg::view_zoom_in())       .NoWantFocus() <<= THISBACK(UserZoomInX);
	horz_out .SetImage(PlotterImg::view_zoom_out())      .NoWantFocus() <<= THISBACK(UserZoomOutX);
	horz_full.SetImage(PlotterImg::view_zoom_horz_full()).NoWantFocus() <<= THISBACK(UserZoomFullX);
	vert_in  .SetImage(PlotterImg::view_zoom_in())       .NoWantFocus() <<= THISBACK(UserZoomInY);
	vert_out .SetImage(PlotterImg::view_zoom_out())      .NoWantFocus() <<= THISBACK(UserZoomOutY);
	vert_full.SetImage(PlotterImg::view_zoom_vert_full()).NoWantFocus() <<= THISBACK(UserZoomFullY);
	full     .SetImage(PlotterImg::view_zoom_full_icon()).NoWantFocus() <<= THISBACK(UserZoomFull);
	AddFrame(*this);
	hscroll <<= THISBACK(OnHScroll);
	vscroll <<= THISBACK(OnVScroll);
	hscroll.WhenLeftClick = vscroll.WhenLeftClick = THISBACK(DoSetFocus);
	hscroll.SetLine(50);
	vscroll.SetLine(50);
}

PlotterCtrl::~PlotterCtrl() {}

void PlotterCtrl::Layout()
{
	Size phys_size(0, 0);
	if(extent.right > extent.left)
		phys_size.cx = fround(fabs(extent.Width() * scale.cx));
	if(extent.bottom > extent.top)
		phys_size.cy = fround(fabs(extent.Height() * scale.cy));
	Size ocli = old_size;
	Size cli = old_size = GetSize();
	phys_size += gap.TopLeft() + gap.BottomRight();
	hscroll.Set(hscroll, cli.cx, phys_size.cx);
	vscroll.Set(vscroll, cli.cy, phys_size.cy);
	if(!IsNull(ocli))
	{
		Size dlog = cli - ocli;
		Pointf pos = delta;
		switch(halign)
		{
		case ALIGN_LEFT:  break;
		default:          pos.x += dlog.cx * 0.5; break;
		case ALIGN_RIGHT: pos.x += dlog.cx; break;
		}
		switch(valign)
		{
		case ALIGN_TOP:    break;
		default:           pos.y += dlog.cy * 0.5; break;
		case ALIGN_BOTTOM: pos.y += dlog.cy; break;
		}
		if(pos != delta)
		{
			SetDelta(pos);
			WhenZoom();
		}
	}
	RefreshBuffer();
}

void PlotterCtrl::FrameAdd(Ctrl& ctrl)
{
	ctrl
	<< (Ctrl&)vscroll << vert_in << vert_out << vert_full
	<< (Ctrl&)hscroll << horz_in << horz_out << horz_full
	<< full;
}

void PlotterCtrl::FrameRemove()
{
	vscroll.Remove();
	vert_in.Remove();
	vert_out.Remove();
	vert_full.Remove();
	hscroll.Remove();
	horz_in.Remove();
	horz_out.Remove();
	horz_full.Remove();
	full.Remove();
}

void PlotterCtrl::FrameLayout(Rect& rc)
{
	int box = (show_scroll ? ScrollBar::GetStdBox() : 0);
	Rect out = rc;
	rc.right -= box;
	rc.bottom -= box;
	int sbx = (aspect ? 0 : box);
	vscroll  .SetFrameRect(rc.right, rc.top, box, rc.Height() - 3 * sbx);
	vert_in  .SetFrameRect(rc.right, rc.bottom - 3 * sbx, box, sbx);
	vert_in  .Enable(sbx);
	vert_out .SetFrameRect(rc.right, rc.bottom - 2 * sbx, box, sbx);
	vert_out .Enable(sbx);
	vert_full.SetFrameRect(rc.right, rc.bottom - sbx, box, sbx);
	hscroll  .SetFrameRect(rc.left, rc.bottom, rc.Width() - 3 * sbx, box);
	horz_in  .SetFrameRect(rc.right - 3 * sbx, rc.bottom, sbx, box);
	horz_in  .Enable(sbx);
	horz_out .SetFrameRect(rc.right - 2 * sbx, rc.bottom, sbx, box);
	horz_out .Enable(sbx);
	horz_full.SetFrameRect(rc.right - sbx, rc.bottom, sbx, box);
	full     .SetFrameRect(rc.right, rc.bottom, box, box);
}

void PlotterCtrl::FrameAddSize(Size& sz)
{
	int box = (show_scroll ? ScrollBar::GetStdBox() : 0);
	sz += box;
}

Rectf PlotterCtrl::FromClient(const Rect& rc) const
{
	if(IsNull(rc))
		return Null;
	Rectf c = (Rectf(rc) - delta) / scale;
	if(scale.cx < 0)
		Swap(c.left, c.right);
	if(scale.cy < 0)
		Swap(c.top,  c.bottom);
	return c;
}

Rect PlotterCtrl::ToClient(const Rectf& rc) const
{
	if(IsNull(rc))
		return Null;
	Rect out = RectfToRect(rc * scale + delta);
	if(scale.cx < 0)
		Swap(out.left, out.right);
	if(scale.cy < 0)
		Swap(out.top, out.bottom);
	return out;
}

double PlotterCtrl::GetAvgScale() const
{
	return (fabs(scale.cx) + fabs(scale.cy)) * 0.5;
}

void PlotterCtrl::SetAspectRatio(double a)
{
	aspect = a;
	ZoomFull();
}

void PlotterCtrl::SetExtent(const Rectf& e)
{
	if(e != extent)
	{
		extent = e;
		Pointf d = delta;
		Layout();
		SetDelta(d);
	}
}

void PlotterCtrl::ScrollInto(Rectf rc)
{
	Rectf view = GetViewRect();
	Sizef shift(0, 0);
	if(rc.Width() > view.Width())
		shift.cx = (rc.left + rc.right - view.left - view.right) / 2;
	else if(rc.left < view.left)
		shift.cx = rc.left - view.left;
	else if(rc.right > view.right)
		shift.cx = rc.right - view.right;
	if(rc.Height() > view.Height())
		shift.cy = (rc.top + rc.bottom - view.top - view.bottom) / 2;
	else if(rc.top < view.top)
		shift.cy = rc.top - view.top;
	else if(rc.bottom > view.bottom)
		shift.cy = rc.bottom - view.bottom;
	if(shift.cx || shift.cy)
	{
		Pointf new_pos = delta - shift * scale;
		AdjustPos(new_pos, Point(Null));
		if(new_pos != delta)
			SetDelta(new_pos);
	}
}

void PlotterCtrl::ScrollInto(Pointf pt)
{
	ScrollInto(Rectf(pt, Sizef(0, 0)));
}

void PlotterCtrl::SetScale(Sizef s)
{
	Size half = GetSize() >> 1;
	Pointf mid = FromClient(Point(half));
	SetZoom(s, Sizef(half) - Sizef(mid) * s);
}

void PlotterCtrl::SetDelta(Pointf d)
{
	LLOG("PlotterCtrl::SetDelta");
	DragHide();
	delta = d;
//	AdjustPos(delta, Null);
	if(!IsNull(extent))
	{
		Rectf sx = extent * scale;
		if(scale.cx < 0) Swap(sx.left, sx.right);
		if(scale.cy < 0) Swap(sx.top, sx.bottom);
		Size sb(gap.left - fround(delta.x + sx.left), gap.top - fround(delta.y + sx.top));
		hscroll.Set(sb.cx);
		vscroll.Set(sb.cy);
		RefreshBuffer();
		UpdateMousePos();
	}
}

void PlotterCtrl::SetZoom(Sizef s, Pointf d)
{
	scale = AdjustScale(s);
	Layout();
	SetDelta(d);
	WhenZoom();
	WhenRescan();
}

void PlotterCtrl::SetZoom(double s, Pointf d)
{
	ASSERT(aspect);
	scale = AdjustScale(Pointf(rev_x ? -s : s, (rev_y ? -s : s) * aspect));
	Layout();
	SetDelta(d);
	WhenZoom();
	WhenRescan();
}

Sizef PlotterCtrl::GetPhysicalZoom() const
{
	Size client = GetSize();
	Size ppm = GetPixelsPerMeter(ScreenInfo());
	Sizef vrc = fpmax(GetViewRect().Size(), Sizef(1e-20, 1e-20));
	return Sizef(client) / Sizef(ppm) / vrc;
}

Sizef PlotterCtrl::AdjustScale(Sizef sc) const
{
	if(fabs(sc.cx) >= max_scale.cx)
		sc.cx = (sc.cx >= 0 ? max_scale.cx : -max_scale.cx);
	if(fabs(sc.cy) >= max_scale.cy)
		sc.cy = (sc.cy >= 0 ? max_scale.cy : -max_scale.cy);
	return sc;
}

void PlotterCtrl::AdjustPos(Pointf& d, Point scpos) const
{
	Rectf c = extent * scale;
	if(rev_x)
		Swap(c.left, c.right);
	if(rev_y)
		Swap(c.top, c.bottom);
	Size sz = GetSize();
	if(c.Width() <= sz.cx - gap.left - gap.right)
		switch(halign)
		{
		case ALIGN_LEFT:   d.x = gap.left - c.left; break;
		default:           d.x = (sz.cx + gap.left - gap.right - c.left - c.right) / 2; break;
		case ALIGN_RIGHT:  d.x = sz.cx - gap.right - c.right; break;
		}
	else
	{
		if(!IsNull(scpos))
			d.x = gap.left - scpos.x - c.left;
		d.x = minmax(d.x, sz.cx - gap.right - c.right, gap.left - c.left);
	}
	if(c.Height() <= sz.cy - gap.top - gap.bottom)
		switch(valign)
		{
		case ALIGN_TOP:    d.y = gap.top - c.top; break;
		default:           d.y = (sz.cy + gap.top - gap.bottom - c.top - c.bottom) / 2; break;
		case ALIGN_BOTTOM: d.y = sz.cy - gap.top - c.bottom; break;
		}
	else
	{
		if(!IsNull(scpos))
			d.y = gap.top - scpos.y - c.top;
		d.y = minmax(d.y, sz.cy - gap.bottom - c.bottom, gap.top - c.top);
	}
}

void PlotterCtrl::AdjustPos(Point scpos, int xymask)
{
	if(!IsNull(extent))
	{
		Pointf d = delta;
		AdjustPos(d, scpos);
		if(xymask & 1) delta.x = d.x;
		if(xymask & 2) delta.y = d.y;
		RefreshBuffer();
		UpdateMousePos();
	}
}

void PlotterCtrl::OnHScroll()
{
	AdjustPos(Point(hscroll, vscroll), 1);
	WhenUserZoom();
}

void PlotterCtrl::OnVScroll()
{
	AdjustPos(Point(hscroll, vscroll), 2);
	WhenUserZoom();
}

Pointf PlotterCtrl::GetSc() const
{
//	if(IsNull(extent))
//		return Pointf(0, 0);
	Pointf sc;
	Size cli = GetSize();
//	if(fabs(extent.Width() * scale.cx) <= cli.cx - gap.left - gap.right)
//		sc.x = (extent.left + extent.right) / 2;
//	else
	sc.x = ((cli.cx >> 1) - delta.x) / scale.cx;
//	if(fabs(extent.Height() * scale.cy) <= cli.cy - gap.top - gap.bottom)
//		sc.y = (extent.top + extent.bottom) / 2;
//	else
	sc.y = ((cli.cy >> 1) - delta.y) / scale.cy;
	return sc;
}

void PlotterCtrl::SetSc(Pointf center)
{
	if(!IsNull(center) && !IsNull(extent))
		SetDelta(Sizef(GetSize() >> 1) - Sizef(center) * scale);
}

void PlotterCtrl::SetZoomSc(Sizef new_scale)
{
	new_scale = AdjustScale(new_scale);
	SetZoom(new_scale, Sizef(GetSize() >> 1) - Sizef(GetSc()) * new_scale);
}

void PlotterCtrl::SetZoomSc(double s)
{
	ASSERT(aspect);
	SetZoomSc(rev_x ? -s : s, (rev_y ? -s : s) * aspect);
}

void PlotterCtrl::ZoomInX()
{
	SetZoomSc(scale.cx * 2, scale.cy);
}

void PlotterCtrl::ZoomOutX()
{
	SetZoomSc(scale.cx / 2, scale.cy);
}

void PlotterCtrl::ZoomX(double min, double max, bool add_gap)
{
	if(min != max)
	{
		int gl = (add_gap ? gap.left : 0), gr = (add_gap ? gap.right : 0);
		int s = GetSize().cx;
		double sc = (rev_x ? -1 : 1) * (s - gl - gr) / (max - min);
		double dx = (rev_x ? s - gr : gl) - sc * min;
		SetZoom(Pointf(sc, scale.cy), Pointf(dx, delta.y));
	}
}

void PlotterCtrl::ZoomInY()
{
	SetZoomSc(scale.cx, scale.cy * 2);
}

void PlotterCtrl::ZoomOutY()
{
	SetZoomSc(scale.cx, scale.cy / 2);
}

void PlotterCtrl::ZoomY(double min, double max, bool add_gap)
{
	if(min != max)
	{
		int gt = (add_gap ? gap.top : 0), gb = (add_gap ? gap.bottom : 0);
		int s = GetSize().cy;
		double sc = (rev_y ? -1 : 1) * (s - gt - gb) / (max - min);
		double dx = (rev_y ? s - gb : gt) - sc * min;
		SetZoom(Pointf(scale.cx, sc), Pointf(delta.x, dx));
	}
}

void PlotterCtrl::Zoom(const Rectf& rc, bool keep_ratio, bool add_gap)
{
	if(rc.right < rc.left && rc.bottom < rc.top)
		return;
	Rectf tmp = rc;
	if(tmp.right  < tmp.left) tmp.left = tmp.right  = 0;
	if(tmp.bottom < tmp.top)  tmp.top  = tmp.bottom = 0;
	Rect g = (add_gap ? gap : Rect(0, 0, 0, 0));
	if(aspect || keep_ratio)
	{
		Size avail = GetSize() - g.TopLeft() - g.BottomRight();
		Sizef size = tmp.Size();
		double d = Abs(size * scale) % Sizef(avail);
		if(d >= 0)
		{
			d /= 2 * avail.cx * fabs(scale.cy);
			tmp.top -= d;
			tmp.bottom += d;
		}
		else
		{
			d /= -2 * avail.cy * fabs(scale.cx);
			tmp.left -= d;
			tmp.right += d;
		}
	}
	if(aspect)
	{
		Size s = GetSize();
		double sx = tmp.Width() <= 1e-20 ? scale.cx : (s.cx - g.left - g.right) / tmp.Width();
		double dx = (rev_x ? s.cx - g.right + sx * tmp.left : g.left - sx * tmp.left);
		double sy = sx * aspect;
		double dy = (rev_y ? s.cy - g.bottom + sy * tmp.top : g.top - sy * tmp.top);
		SetZoom(sx, Pointf(dx, dy));
//		AdjustPos(Null);
	}
	else
	{
		if(rc.left < rc.right)  ZoomX(tmp.left, tmp.right);
		if(rc.top  < rc.bottom) ZoomY(tmp.top,  tmp.bottom);
	}
}

ImageDraw& PlotterCtrl::BeginBufferPaint()
{
	LLOGBLOCK("PlotterCtrl::BeginBufferPaint");
	ASSERT(!is_painting);
	paint_buffer = Null;
	paint_buffer = Image(max(GetSize(), Size(1, 1)));
	LLOG("-> size = " << paint_buffer.GetSize());
	paint_draw.Open(ScreenInfo(), paint_buffer);
	paint_draw.DrawRect(GetSize(), background);
	is_painting = true;
	abort_repaint = false;
	return paint_draw;
}

void PlotterCtrl::EndBufferPaint()
{
	LLOGBLOCK("PlotterCtrl::EndBufferPaint");
	ASSERT(is_painting);
	Plotter plotter(paint_draw, scale, delta);
	plotter.PathMap(&PathStyleMap::App());
	if(drag_drop)
	{
		lock_drag_drop = true;
		drag_drop->Plot(plotter);
		lock_drag_drop = false;
	}
	if(short_drag_drop)
	{
		lock_short_drag_drop = true;
		short_drag_drop->Plot(plotter);
		lock_short_drag_drop = false;
	}
	paint_draw.Close();
	if(abort_repaint)
		paint_buffer = Null;
	is_painting = false;
	PostRefresh();
}

void PlotterCtrl::PostRefresh()
{
	tcb_refresh.KillSet(1, THISBACK(Refresh0));
}

void PlotterCtrl::Paint(Draw& draw)
{
	LOGBLOCK("PlotterCtrl::Paint");
	RLOG("PlotterCtrl::Paint @ " << GetSysTime());
	bool shown = IsDragging();
	DragHide();
	Rect clip = draw.GetClip();
	if(is_painting) {
#ifdef PLATFORM_WIN32
		if(!paint_buffer.IsEmpty()) {
			LLOG("-> blit paint_buffer");
			if(!BitBlt(draw, Point(0, 0), paint_draw, paint_buffer.GetRect())) {
				LLOG("-> blit error");
			}
		}
#endif
	}
	else {
		if(buffer_paint && !draw.Dots()) {
			Size size = GetSize();
			if(paint_buffer.IsEmpty() || paint_buffer.GetSize() != size) {
				LLOG("-> refresh paint buffer");
				Draw& idraw = BeginBufferPaint();
				Plotter plotter(idraw, scale, delta);
				plotter.PathMap(&PathStyleMap::App());
				Plot(plotter);
				EndBufferPaint();
			}
			else {
				LLOG("-> DrawImage paint_buffer");
				draw.DrawImage(0, 0, paint_buffer);
			}
		}
		else {
			LLOG("-> Plot (direct)");
			draw.DrawRect(clip, background);
			Plotter plotter(draw, scale, delta);
			plotter.PathMap(&PathStyleMap::App());
			Plot(plotter);
			if(drag_drop) {
				lock_drag_drop = true;
				drag_drop->Plot(plotter);
				lock_drag_drop = false;
			}
			if(short_drag_drop) {
				lock_short_drag_drop = true;
				short_drag_drop->Plot(plotter);
				lock_short_drag_drop = false;
			}
		}
	}
	if(shown)
		DragShow();
}

/*
void PlotterCtrl::AsyncPaint()
{
	if(paint_draw && !paint_buffer.IsEmpty())
	{
//		ViewDraw vdraw(this);
//		BitBlt(vdraw, Point(0, 0), *paint_draw, paint_buffer.GetRect());
	}
}
*/

void PlotterCtrl::RefreshBuffer()
{
	LLOGBLOCK("PlotterCtrl::RefreshBuffer");
	if(IsBufferPaint()) {
		if(!is_painting)
			paint_buffer.Clear();
		else {
			abort_repaint = true;
			AbortPlot();
		}
	}
	Refresh();
}

void PlotterCtrl::RefreshBuffer(const Rect& rc)
{
	LLOGBLOCK("PlotterCtrl::RefreshBuffer");
	if(IsBufferPaint()) {
		if(!is_painting)
			paint_buffer.Clear();
		else {
			abort_repaint = true;
			AbortPlot();
		}
	}
	Refresh(rc);
}

Image PlotterCtrl::CursorImage(Point pt, dword keyflags)
{
	Image out = Image::Arrow();
	if(short_drag_drop)
	{
		lock_short_drag_drop = true;
		out = short_drag_drop->Cursor(FromClient(pt), keyflags, IsDragging());
		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
		lock_drag_drop = true;
		out = drag_drop->Cursor(FromClient(pt), keyflags, IsDragging());
		lock_drag_drop = false;
	}
	return out;
}

void PlotterCtrl::SyncPush()
{
	push_scale = scale;
	push_delta = delta;
}

bool PlotterCtrl::Push(Point pt, dword keyflags)
{
	bool push = false;
	SyncPush();
	drag_start = FromPushClient(pt);
	if(short_drag_drop)
	{
//		lock_short_drag_drop = true;
		push = short_drag_drop->Push(drag_start, keyflags);
//		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
//		lock_drag_drop = true;
		push = drag_drop->Push(drag_start, keyflags);
//		lock_drag_drop = false;
	}
	return push;
}

void PlotterCtrl::Drag(Point start, Point prev, Point curr, dword keyflags)
{
	LOG("PlotterCtrl::Drag(" << start << "; prev = " << prev << ", curr = " << curr << ", flags = " << FormatIntHex(keyflags));
//	RLOG("PlotterCtrl::Drag, short = " << ~short_drag_drop << ", " << (~short_drag_drop
//		? typeid(*short_drag_drop).name() : "NULL") << ", long = " << ~drag_drop << ", "
//		<< (~drag_drop ? typeid(*drag_drop).name() : "NULL"));
	if(short_drag_drop)
	{
		lock_short_drag_drop = true;
//		RDUMP(~short_drag_drop);
		short_drag_drop->Drag(drag_start, FromPushClientNull(prev), FromPushClientNull(curr), keyflags);
		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
		lock_drag_drop = true;
//		RLOG("PlotterCtrl::Drag->drag_drop::Drag");
		drag_drop->Drag(drag_start, FromPushClientNull(prev), FromPushClientNull(curr), keyflags);
//		RLOG("//PlotterCtrl::Drag->drag_drop::Drag");
		lock_drag_drop = false;
	}
//	RLOG("//PlotterCtrl::Drag");
}

void PlotterCtrl::Drop(Point start, Point end, dword keyflags)
{
	if(short_drag_drop)
	{
//		lock_short_drag_drop = true;
		short_drag_drop->Drop(drag_start, FromPushClient(end), keyflags);
//		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
		lock_drag_drop = true;
		drag_drop->Drop(drag_start, FromPushClient(end), keyflags);
		lock_drag_drop = false;
	}
}

void PlotterCtrl::Click(Point pt, dword keyflags)
{
	if(short_drag_drop)
	{
//		lock_short_drag_drop = true;
		short_drag_drop->Click(FromPushClient(pt), keyflags);
//		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
		lock_drag_drop = true;
		drag_drop->Click(FromPushClient(pt), keyflags);
		lock_drag_drop = false;
	}
}

void PlotterCtrl::Cancel()
{
	if(short_drag_drop)
	{
		lock_short_drag_drop = true;
		short_drag_drop->Cancel();
		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
		lock_drag_drop = true;
		drag_drop->Cancel();
		lock_drag_drop = false;
	}
}

bool PlotterCtrl::Key(dword key, int repcnt)
{
	bool dd_key = false;
	if(short_drag_drop)
	{
		lock_short_drag_drop = true;
		dd_key = short_drag_drop->Key(key);
		lock_short_drag_drop = false;
	}
	else if(drag_drop)
	{
		lock_drag_drop = true;
		dd_key = drag_drop->Key(key);
		lock_drag_drop = false;
	}
	if(dd_key)
		return true;
	if(hscroll.HorzKey(key))
	{
		OnHScroll();
		return true;
	}
	if(vscroll.VertKey(key))
	{
		OnVScroll();
		return true;
	}
	switch(key)
	{
	case K_ADD:      ZoomIn();  return true;
	case K_SUBTRACT: ZoomOut(); return true;
	}
	return DragDropCtrl::Key(key, repcnt);
}

void PlotterCtrl::MouseWheel(Point p, int zdelta, dword keyflags)
{
	if(keyflags & K_SHIFT)
	{
		hscroll.Wheel(zdelta, 3);
		OnHScroll();
	}
	else
	{
		vscroll.Wheel(zdelta, 3);
		OnVScroll();
	}
}

void PlotterCtrl::MouseMove(Point pt, dword keyflags)
{
	UpdateMousePos();
	DragDropCtrl::MouseMove(pt, keyflags);
}

void PlotterCtrl::UpdateMousePos()
{
	mouse_pos = FromClient(::GetMousePos() - Size(GetScreenView().TopLeft()));
	RefreshPos();
	WhenMousePos();
}

void PlotterCtrl::PickShortDragDrop(One<PlotterDragDrop> sdd)
{
	ASSERT(!lock_short_drag_drop);
	DragStop();
	short_drag_drop = sdd;
	RefreshDragDrop();
	WhenRescan();
}

One<PlotterDragDrop> PlotterCtrl::ClearShortDragDrop()
{
	ASSERT(!lock_short_drag_drop);
	One<PlotterDragDrop> sdd = short_drag_drop;
	short_drag_drop.Clear();
	RefreshDragDrop();
	WhenRescan();
	return sdd;
}

void PlotterCtrl::EndShortDragDrop()
{
	if(drag_drop && short_drag_drop)
		ClearShortDragDrop();
}

void PlotterCtrl::PickDragDrop(One<PlotterDragDrop> dd)
{
	ASSERT(!lock_short_drag_drop && !lock_drag_drop);
	DragStop();
	short_drag_drop.Clear();
	drag_drop = dd;
	RefreshDragDrop();
	WhenRescan();
}

One<PlotterDragDrop> PlotterCtrl::ClearDragDrop()
{
	ASSERT(!lock_drag_drop);
	One<PlotterDragDrop> out = drag_drop;
	drag_drop.Clear();
	RefreshDragDrop();
	WhenRescan();
	return out;
}

void PlotterCtrl::ToolView(Bar& bar)
{
	if(aspect)
	{
		ToolViewZoomIn(bar);
		ToolViewZoomOut(bar);
		ToolViewZoomFull(bar);
		bar.Separator();
	}
	else
	{
		ToolViewZoomInX(bar);
		ToolViewZoomOutX(bar);
		ToolViewZoomFullX(bar);
		bar.MenuSeparator();
		ToolViewZoomInY(bar);
		ToolViewZoomOutY(bar);
		ToolViewZoomFullY(bar);
		bar.MenuSeparator();
		ToolViewZoomIn(bar);
		ToolViewZoomOut(bar);
		ToolViewZoomFull(bar);
	}
	ToolViewPan(bar);
}

void PlotterCtrl::ToolViewZoomInX(Bar& bar)
{
	bar.AddMenu(t_("Zoom in horz."), PlotterImg::view_zoom_in(), THISBACK(OnViewZoomInX))
		.Help(t_("Zoom in horizontally"));
}

void PlotterCtrl::OnViewZoomInX()
{
	UserZoomInX();
}

void PlotterCtrl::ToolViewZoomOutX(Bar& bar)
{
	bar.AddMenu(t_("Zoom out horz."), PlotterImg::view_zoom_out(), THISBACK(OnViewZoomOutX))
		.Help(t_("Zoom out horizontally"));
}

void PlotterCtrl::OnViewZoomOutX()
{
	UserZoomOutX();
}

void PlotterCtrl::ToolViewZoomFullX(Bar& bar)
{
	bar.AddMenu(t_("Zoom full horz."), PlotterImg::view_zoom_full(), THISBACK(OnViewZoomFullX))
		.Help(t_("Display full x axis range in view"));
}

void PlotterCtrl::OnViewZoomFullX()
{
	UserZoomFullX();
}

void PlotterCtrl::ToolViewZoomInY(Bar& bar)
{
	bar.AddMenu(t_("Zoom in vert."), PlotterImg::view_zoom_in(), THISBACK(OnViewZoomInY))
		.Help(t_("Zoom in vertically"));
}

void PlotterCtrl::OnViewZoomInY()
{
	UserZoomInY();
}

void PlotterCtrl::ToolViewZoomOutY(Bar& bar)
{
	bar.AddMenu(t_("Zoom out vert."), PlotterImg::view_zoom_out(), THISBACK(OnViewZoomOutY))
		.Help(t_("Zoom out vertically"));
}

void PlotterCtrl::OnViewZoomOutY()
{
	UserZoomOutY();
}

void PlotterCtrl::ToolViewZoomFullY(Bar& bar)
{
	bar.AddMenu(t_("Zoom full vert."), PlotterImg::view_zoom_full(), THISBACK(OnViewZoomFullY))
		.Help(t_("Display full y axis range in view"));
}

void PlotterCtrl::OnViewZoomFullY()
{
	UserZoomFullY();
}

void PlotterCtrl::ToolViewZoomOut(Bar& bar)
{
	bar.Add(t_("Zoom out"), PlotterImg::view_zoom_out(), THISBACK(OnViewZoomOut))
		.Check(IsDragDrop<ZoomOutDragDrop>(this))
		.Help(t_("Zoom out current view"));
}

void PlotterCtrl::OnViewZoomOut()
{
	if(!IsDragDrop<ZoomOutDragDrop>(this))
		PickShortDragDrop(new ZoomOutDragDrop(*this));
	else if(drag_drop)
		EndShortDragDrop();
	else
		UserZoomOut();
}

void PlotterCtrl::ToolViewZoomFull(Bar& bar)
{
	bar.Add(t_("Zoom full"), PlotterImg::view_zoom_full(), THISBACK(OnViewZoomFull))
		.Help(t_("Zoom everything into view"));
}

void PlotterCtrl::OnViewZoomFull()
{
	UserZoomFull();
}

void PlotterCtrl::ToolViewZoomIn(Bar& bar)
{
	bar.Add(t_("Zoom in"), PlotterImg::view_zoom_in(), THISBACK(OnViewZoomIn))
		.Check(IsDragDrop<ZoomInDragDrop>(this))
		.Help(t_("Zoom in current view (click to zoom in 2x, drag & drop to zoom in area)"));
}

void PlotterCtrl::OnViewZoomIn()
{
	if(!IsDragDrop<ZoomInDragDrop>(this))
		PickShortDragDrop(new ZoomInDragDrop(*this));
	else if(drag_drop)
		EndShortDragDrop();
	else
		UserZoomIn();
}

void PlotterCtrl::ToolViewPan(Bar& bar)
{
	bar.Add(t_("Pan"), PlotterImg::view_pan(), THISBACK(OnViewPan))
		.Check(IsDragDrop<PanDragDrop>(this))
		.Help(t_("Drag & drop view position"));
}

void PlotterCtrl::OnViewPan()
{
	if(!IsDragDrop<PanDragDrop>(this))
		PickShortDragDrop(new PanDragDrop(*this));
	else
		EndShortDragDrop();
}

void PlotterCtrl::UserZoomInX()
{
	ZoomInX();
	WhenUserZoom();
}

void PlotterCtrl::UserZoomOutX()
{
	ZoomOutX();
	WhenUserZoom();
}

void PlotterCtrl::UserZoomFullX()
{
	if(extent.left < extent.right)
	{
		ZoomX(extent.left, extent.right);
		WhenUserZoom();
	}
}

void PlotterCtrl::UserZoomX(double min, double max)
{
	ZoomX(min, max);
	WhenUserZoom();
}

void PlotterCtrl::UserZoomInY()
{
	ZoomInY();
	WhenUserZoom();
}

void PlotterCtrl::UserZoomOutY()
{
	ZoomOutY();
	WhenUserZoom();
}

void PlotterCtrl::UserZoomFullY()
{
	if(extent.top < extent.bottom)
	{
		ZoomY(extent.top, extent.bottom);
		WhenUserZoom();
	}
}

void PlotterCtrl::UserZoomY(double min, double max)
{
	ZoomY(min, max);
	WhenUserZoom();
}

void PlotterCtrl::UserZoomIn()
{
	ZoomInX();
	ZoomInY();
	WhenUserZoom();
}

void PlotterCtrl::UserZoomOut()
{
	ZoomOutX();
	ZoomOutY();
	WhenUserZoom();
}

void PlotterCtrl::UserZoomFull()
{
	Zoom(extent, false);
	WhenUserZoom();
}

void PlotterCtrl::UserZoom(const Rectf& rc, bool keep_ratio)
{
	Zoom(rc, keep_ratio);
	WhenUserZoom();
}

//////////////////////////////////////////////////////////////////////
// PlotterDragDrop::

void PlotterDragDrop::Drag(Pointf pt, Pointf prev, Pointf curr, dword keyflags)
{
	Rectf rc_prev = (IsNull(prev) ? Rectf(Null) : SortRectf(pt, prev));
	Rectf rc_curr = (IsNull(curr) ? Rectf(Null) : SortRectf(pt, curr));
	if(rc_prev != rc_curr)
		DragRect(rc_prev, rc_curr, keyflags);
}

void PlotterDragDrop::DragRect(const Rectf& prev, const Rectf& curr, dword keyflags)
{
	PlotterCtrl& ctrl = GetOwner();
	PlotterCtrl::ViewPlot plot(ctrl);
	PlotDragRect(plot, prev);
	PlotDragRect(plot, curr);
}

void PlotterDragDrop::Drop(Pointf pt, Pointf end, dword keyflags)
{
	DropRect(SortRectf(pt, end), keyflags);
}

//////////////////////////////////////////////////////////////////////
// ZoomInDragDrop::

Image ZoomInDragDrop::Cursor(Pointf pt, dword keyflags, bool dragging) const
{
	return keyflags & K_CTRL && !owner.IsAspectRatio()
		? PlotterImg::view_zoom_in_aniso() : PlotterImg::view_zoom_in();
}

void ZoomInDragDrop::DropRect(const Rectf& rc, dword keyflags)
{
	owner.Zoom(rc, !(keyflags & K_CTRL) || owner.IsAspectRatio());
	owner.WhenUserZoom();
	owner.EndShortDragDrop();
}

void ZoomInDragDrop::Click(Pointf pt, dword keyflags)
{
	PlotterCtrl& owner = GetOwner();
	Pointf d = Sizef(owner.GetSize()) / Abs(4.0 * owner.GetScale());
	owner.Zoom(Rectf(pt - d, pt + d), true);
	owner.WhenUserZoom();
}

//////////////////////////////////////////////////////////////////////
// ZoomOutDragDrop::

Image ZoomOutDragDrop::Cursor(Pointf pt, dword keyflags, bool dragging) const
{
	return PlotterImg::view_zoom_out();
}

bool ZoomOutDragDrop::Push(Pointf pt, dword keyflags)
{
	Rectf rc = GetOwner().GetViewRect();
	rc.Inflate(rc.Size() * 0.5);
	owner.Zoom(rc + pt - rc.CenterPoint());
	owner.WhenUserZoom();
	owner.EndShortDragDrop();
	return false;
}

//////////////////////////////////////////////////////////////////////
// PanDragDrop::

Image PanDragDrop::Cursor(Pointf pt, dword keyflags, bool dragging) const
{
	return PlotterImg::view_pan();
}

bool PanDragDrop::Push(Pointf pt, dword keyflags)
{
	old_delta = GetOwner().GetPushDelta();
	return true;
}

void PanDragDrop::Drag(Pointf start, Pointf prev, Pointf curr, dword keyflags)
{
	PlotterCtrl& owner = GetOwner();
	if(!IsNull(curr))
	{
		Pointf p = old_delta + (curr - start) * owner.GetPushScale();
		owner.AdjustPos(p, Point(Null));
		if(p != owner.GetDelta()) {
			owner.SetDelta(p);
			owner.WhenUserZoom();
		}
	}
}

void PanDragDrop::Drop(Pointf start, Pointf end, dword keyflags)
{
	PlotterCtrl& owner = GetOwner();
//	owner.SetDelta(GetDelta(start, end));
	owner.WhenUserZoom();
	owner.EndShortDragDrop();
}

void PanDragDrop::Cancel()
{
	GetOwner().SetDelta(old_delta);
}

Pointf PanDragDrop::GetDelta(Pointf start, Pointf pt)
{
	if(IsNull(pt))
		return Null;
	const PlotterCtrl& owner = GetOwner();
	Pointf p = owner.GetDelta() + (pt - start) * owner.GetScale();
	owner.AdjustPos(p, Null);
	return p;
}
