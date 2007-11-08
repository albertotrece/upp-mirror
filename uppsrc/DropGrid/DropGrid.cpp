#include "DropGrid.h"

NAMESPACE_UPP

DropGrid::PopUpGrid::PopUpGrid()
{
	LiveCursor();
	FullColResizing(false);
	HorzGrid(false);
	DrawLastVertLine(false);
	TabAddsRow(false);
	SearchMoveCursor(true);
	SearchHideRows(false);
	SearchDisplay(false);
	ResizePanel();
	WhenLeftClick = THISBACK(CloseData);
	WhenEnter = THISBACK(CloseData);
	WhenEscape = THISBACK(CloseNoData);
	GridCtrl::WhenClose = THISBACK(CloseNoData);
}

void DropGrid::PopUpGrid::PopUp(Ctrl *owner, const Rect &r)
{
	Close();
	SetRect(r);
	Ctrl::PopUp(owner, true, true, GUI_DropShadows());
	SetFocus();
}

void DropGrid::PopUpGrid::CloseNoData()
{
	WhenCloseNoData();
	Deactivate();
}

void DropGrid::PopUpGrid::CloseData()
{
	WhenCloseData();
	Deactivate();
}

void DropGrid::PopUpGrid::Deactivate()
{
	if(IsOpen() && IsPopUp())
	{
		WhenPopDown();
		IgnoreMouseClick();
		GridCtrl::Close();
		WhenClose();
	}
}

DropGrid::DropGrid()
{
	list.WhenCloseData = THISBACK(CloseData);
	list.WhenCloseNoData = THISBACK(CloseNoData);
	list.WhenClose = THISBACK(Close);
	list.WhenSearchCursor = THISBACK(SearchCursor);
	list.BackPaint();
	drop.AddButton().Main().WhenPush = THISBACK(Drop);
	drop.SetStyle(drop.StyleFrame());
	drop.NoDisplay();
	drop.AddTo(*this);
	//SetFrame(EditFieldFrame());
	EnableDrop(false);
	list_width = 0;
	list_height = 0;
	drop_lines = 16;
	display_all = false;
	header = true;
	valuekey = false;
	key_col = 0;
	value_col = -1;
	rowid = -1;
	notnull = false;
	format_columns = true;
	drop_enter = false;
	data_action = false;
	searching = true;
	display = this;
}

void DropGrid::Close()
{
	Rect r = list.GetRect();
	list_width = r.Width();
	list_height = r.Height();
	SetFocus();
}

void DropGrid::CloseData()
{
	UpdateValue();
	DoAction(list.GetCursor());
}

void DropGrid::CloseNoData()
{
	if(list.GetRowId() != rowid)
		list.SetCursorId(rowid);
}

void DropGrid::Drop()
{
	if(!IsEditable())
		return;

	GridDisplay &dsp = list.GetDisplay();
	if(!header)
	{
		list.HideRow(0, false);
		dsp.SetHorzMargin(2, 2);
	}
	else
		dsp.SetHorzMargin();
		
	list.UpdateRows(true); //TODO: try to avoid it..

	Rect rs = GetScreenRect();
	int width = rs.Width();
	int resize_height = list.GetResizePanelHeight();
	int height = list.GetHeight() + 4 + resize_height;
	int drop_height = drop_lines * GridCtrl::GD_ROW_HEIGHT + header * GridCtrl::GD_HDR_HEIGHT + 4 + resize_height;
	if(!display_all && height > drop_height)
		height = drop_height;

	list.resize_panel.SetMinSize(Size(width, height));

	if(list_width > width)
		width = list_width;
	if(list_height > height)
		height = list_height;

	Rect rw = Ctrl::GetWorkArea();
	Rect r;
	r.left   = rs.left;
	r.right  = rs.left + width;
	r.top    = rs.bottom;
	r.bottom = rs.bottom + height;

	if(r.bottom > rw.bottom)
	{
		r.top = rs.top - height;
		r.bottom = rs.top;
	}
	if(r.right > rw.right)
	{
		int diff;
		if(rs.right <= rw.right)
			diff = rs.right - r.right;
		else
			diff = rw.right - r.right;

		r.left += diff;
		r.right += diff;
	}
	if(r.left < rw.left)
	{
		int diff = rw.left - r.left;
		r.left += diff;
		r.right += diff;

	}

	if(searching)
		list.ClearFound();
	list.PopUp(this, r);
	list.CenterCursor();
}


void DropGrid::Paint(Draw& w)
{
	Size sz = GetSize();
	w.DrawRect(sz, SColorPaper());
	GridDisplay &disp = display ? *display : list.GetDisplay();
	bool hf = HasFocus();
	bool isnull = rowid < 0;
	Color fg = hf ? SColorHighlightText() : IsEnabled() ? SColorText() : SColorDisabled();
	Color bg = !IsEnabled() ? SColorFace
	                        : notnull && isnull ? Blend(SColorPaper, Color(255, 0, 0), /*hf ? 55 :*/ 32)
	                                            : hf ? SColorHighlight() : SColorPaper();

	const int d = 1;
	if(isnull)
		w.DrawRect(d, d, sz.cx - d * 2, sz.cy - d * 2, bg);
	else
	{
		Font fnt(StdFont());
		Paint0(w, 1, 1, d, d, sz.cx - d * 2, sz.cy - d * 2, Format0(Null, rowid), 0, fg, bg/*SColorPaper()*/, fnt);
	}
	
	if(hf)
		DrawFocus(w, d - 1, d - 1, sz.cx - (d - 1) * 2, sz.cy - (d - 1) * 2);
}


void DropGrid::LeftDown(Point p, dword keyflags)
{
	Drop();
}

void DropGrid::GotFocus()
{
	Refresh();
}

void DropGrid::LostFocus()
{
	Refresh();
}

void DropGrid::Paint0(Draw &w, int lm, int rm, int x, int y, int cx, int cy, const Value &val, dword style, Color &fg, Color &bg, Font &fnt, bool found, int fs, int fe)
{
	w.DrawRect(x, y, cx, cy, bg);
	int nx = x + lm;
	int ny = y + tm;
	int ncx = cx - lm - rm;

	if(IsType< Vector<String> >(val))
	{
		const Vector<String> &v = ValueTo< Vector<String> >(val);
		const char * SPACE = " ";
		
		int tcx = 0;
		int scx = GetTextSize(SPACE, fnt).cx;

		int cnt = v.GetCount();
		Size isz = GridImg::Dots2().GetSize();
		for(int i = 0; i < cnt; i++)
		{
			fnt.Bold((i + 1) & 1);
			Size tsz = GetTextSize(v[i], fnt);
			DrawText(w, nx, x + lm + tcx, 
			         ny, tcx + tsz.cx > ncx - isz.cx ? ncx - tcx: tsz.cx + isz.cx, cy,
			         GD::VCENTER, WString(v[i]), fnt, fg, bg, found, fs, fe, false);
			tcx += tsz.cx + scx;
		}
	}
	else
		DrawText(w, nx, nx, ny, ncx, cy, GD::VCENTER, GetStdConvertedValue(val), fnt, fg, bg, found, fs, fe, false);
}


void DropGrid::Paint(Draw &w, int x, int y, int cx, int cy, const Value &val, dword style, Color &fg, Color &bg, Font &fnt, bool found, int fs, int fe)
{
	Paint0(w, lm, rm, x, y, cx, cy, val, style, fg, bg, fnt, found, fs, fe);
}

DropGrid& DropGrid::Width(int w)
{
	list_width = w;
	return *this;
}

DropGrid& DropGrid::Height(int h)
{
	list_height = h;
	return *this;
}

DropGrid& DropGrid::SetKeyColumn(int n)
{
	key_col = n;
	return *this;
}

DropGrid& DropGrid::SetValueColumn(int n)
{
	value_col = n;
	return *this;
}

DropGrid& DropGrid::AddValueColumn(int n)
{
	value_cols.Add(n);
	return *this;
}

DropGrid& DropGrid::AddValueColumns(int first /* = -1*/, int last /* = -1*/)
{
	int s = first < 0 ? 0: first;
	int e = last < 0 ? list.GetColumnCount() - 1: last;
	for(int i = s; i <= e; i++)
		 value_cols.Add(i);

	return *this;
}

DropGrid& DropGrid::DisplayAll(bool b)
{
	display_all = b;
	return *this;
}

DropGrid& DropGrid::SetDropLines(int d)
{
	drop_lines = d;
	return *this;
}

DropGrid& DropGrid::Header(bool b /* = true*/)
{
	header = b;
	return *this;
}

DropGrid& DropGrid::NoHeader()
{
	return Header(false);
}

DropGrid& DropGrid::Resizeable(bool b /* = true*/)
{
	list.ResizePanel(b);
	return *this;
}

DropGrid& DropGrid::ColorRows(bool b)
{
	list.ColorRows(b);
	return *this;
}

DropGrid& DropGrid::NotNull(bool b)
{
	notnull = b;
	return *this;
}

DropGrid& DropGrid::ValueAsKey(bool b)
{
	valuekey = b;
	return *this;
}

DropGrid& DropGrid::SetDisplay(GridDisplay &d)
{
	display = &d;
	return *this;
}

DropGrid& DropGrid::FormatColumns(bool b /* = true*/)
{
	format_columns = b;
	return *this;
}

DropGrid& DropGrid::DropEnter(bool b /* = true*/)
{
	drop_enter = b;
	return *this;
}

DropGrid& DropGrid::DataAction(bool b /* = true*/)
{
	data_action = b;
	return *this;
}

DropGrid& DropGrid::Searching(bool b /* = true*/)
{
	searching = b;
	return *this;
}

int DropGrid::GetCount() const
{
	return list.GetCount();
}

Value DropGrid::GetData() const
{
	return valuekey ? value
	                : rowid >= 0 ? list.Get(key_col) : notnull ? NotNullError() : Null;
}

Value DropGrid::GetValue() const
{
	return value;
}

Value DropGrid::GetKey() const
{
	return rowid >= 0 ? list.Get(key_col) : Null;
}

void DropGrid::UpdateValue()
{
	if(!list.IsCursor())
		return;

	if(value_cols.GetCount() > 0)
		value = MakeLongValue();
	else
		value = list.Get(value_col);
}

void DropGrid::SetData(const Value& v)
{
	int row = list.Find(v, key_col);
	if(row >= 0)
	{
		list.SetCursor(row);
		UpdateValue();
		DoAction(row, data_action);
		Refresh();
	}
	else
		ClearValue();
}

void DropGrid::DoAction(int row, bool action)
{
	int rid = list.GetRowId(row);
	if(rid != rowid)
	{
		rowid = rid;
		if(action)
			WhenAction();
	}
}

GridCtrl::ItemRect& DropGrid::AddColumn(const char *name, int width, bool idx)
{
	return list.AddColumn(name, width, idx);
}

GridCtrl::ItemRect& DropGrid::AddColumn(Id id, const char *name, int width, bool idx)
{
	return list.AddColumn(id, name, width, idx);
}

GridCtrl::ItemRect& DropGrid::AddIndex(const char *name)
{
	return list.AddIndex(name);
}

GridCtrl::ItemRect& DropGrid::AddIndex(Id id)
{
	return list.AddIndex(id);
}

MultiButton::SubButton& DropGrid::AddButton(int type, Callback &cb)
{
	MultiButton::SubButton& btn = drop.InsertButton(1);
	switch(type)
	{
		case BTN_PLUS:
			btn.SetImage(GridImg::SelPlus);
			break;				
		case BTN_SELECT:
			btn.SetImage(GridImg::SelDots);
			break;
		case BTN_LEFT:
			btn.SetImage(GridImg::SelLeft);
			break;
		case BTN_RIGHT:
			btn.SetImage(GridImg::SelRight);
			break;
		case BTN_UP:
			btn.SetImage(GridImg::SelUp);
			break;
		case BTN_DOWN:
			btn.SetImage(GridImg::SelDn);
			break;
	}
	btn.WhenPush = cb;
	return btn;
}

MultiButton::SubButton& DropGrid::AddSelect(Callback &cb)
{
	return AddButton(BTN_SELECT, cb);
}

MultiButton::SubButton& DropGrid::AddPlus(Callback &cb)
{
	return AddButton(BTN_PLUS, cb);
}

MultiButton::SubButton& DropGrid::AddEdit(Callback &cb)
{
	return AddButton(BTN_RIGHT, cb);
}

MultiButton::SubButton& DropGrid::GetButton(int n)
{
	return drop.GetButton(n);
}

void DropGrid::ClearValue()
{
	value = Null;
	rowid = -1;
	list.ClearCursor();
	WhenAction();
	Refresh();
}

void DropGrid::Reset()
{
	list.Reset();
	ClearValue();
	value_cols.Clear();
	EnableDrop(false);
}

void DropGrid::Clear()
{
	list.Clear();
	ClearValue();
	EnableDrop(false);
}

Value DropGrid::Get(int c) const
{
	return list.Get(c);
}

Value DropGrid::Get(Id id) const
{
	return list.Get(id);
}

Value DropGrid::Get(int r, int c) const
{
	return list.Get(r, c);
}

Value DropGrid::Get(int r, Id id) const
{
	return list.Get(r, id);
}

void DropGrid::Set(int c, const Value& v)
{
	list.Set(c, v);
}

void DropGrid::Set(Id id, const Value& v)
{
	list.Set(id, v);
}

void DropGrid::Set(int r, int c, const Value& v)
{
	list.Set(r, c, v);
}

void DropGrid::Set(int r, Id id, const Value& v)
{
	list.Set(r, id, v);
}

void DropGrid::Set(int r, const Vector<Value> &v, int data_offset, int column_offset)
{
	list.Set(r, v, data_offset, column_offset);
}

void DropGrid::Add(const Vector<Value> &v, int data_offset, int column_offset)
{
	list.AddRow();
	list.Set(v, data_offset, column_offset);
	EnableDrop();
}

Value& DropGrid::operator() (int r, int c)
{
	return list(r, c);
}

Value& DropGrid::operator() (int c)
{
	return list(c);
}

Value& DropGrid::operator() (Id id)
{
	return list(id);
}

Value& DropGrid::operator() (int r, Id id)
{
	return list(r, id);
}

GridCtrl::ItemRect& DropGrid::GetRow(int r)
{
	return list.GetRow(r);
}

int DropGrid::Find(const Value& v, int col)
{
	return list.Find(v, col);
}

int DropGrid::Find(const Value& v, Id id)
{
	return list.Find(v, id);
}

void DropGrid::CancelUpdate()
{
	int prevrow = list.GetPrevCursor();
	if(prevrow >= 0)
	{
		list.SetCursor(prevrow);
		UpdateValue();
		rowid = list.GetRowId(prevrow);
		Refresh();
	}
	else
		ClearValue();
}

void DropGrid::Change(int dir)
{
	int c = -1;
	if(!list.IsCursor())
	{
		if(dir < 0)
			list.GoEnd();
		else
			list.GoBegin();

		c = list.GetCursor();
	}
	else
		c = list.GetCursor() + dir;

	if(list.IsValidCursor(c))
	{
		list.SetCursor(c);
		UpdateValue();
		DoAction(c);
	}

	Refresh();
}

void DropGrid::SearchCursor()
{
	value = list.Get(value_col);
	rowid = list.GetRowId();
	Refresh();
}

bool DropGrid::Key(dword k, int)
{
	if(IsReadOnly()) return false;

	if(drop_enter && k == K_ENTER)
		k = K_ALT_DOWN;

	switch(k)
	{
		case K_ALT_DOWN:
			Drop();
			break;
		case K_DOWN:
		case K_RIGHT:
			Change(1);
			break;
		case K_UP:
		case K_LEFT:
			Change(-1);
			break;
		case K_DELETE:
			ClearValue();
			break;
		default:
			if(searching && k >= 32 && k < 65536)
			{
				if(!list.IsOpen())
					Drop();
				list.Search(k);
			}
			return false;
	}
	return true;
}

int DropGrid::AddColumns(int cnt)
{
	if(!list.GetColumnCount())
	{
		if(cnt <= 2)
		{
			NoHeader();
			Resizeable(false);
		}
		if(cnt == 1)
		{
			list.AddColumn();
			key_col = value_col = 0;
		}
		else if(cnt == 2)
		{
			list.AddIndex();
			list.AddColumn();
			key_col = 0;
			value_col = 1;
		}
		else
			for(int i = 0; i < cnt; i++)
				list.AddColumn();
	}
	return list.GetCount();
}

int DropGrid::SetIndex(int n)
{
	int r = rowid;
	n = list.GetRowId(n);
	if(n >= 0)
	{
		list.SetCursor(n);
		UpdateValue();
		DoAction(n);
	}
	return r;
}

int DropGrid::GetIndex() const
{
	return rowid;
}

bool DropGrid::IsSelected()
{
	return rowid >= 0;
}

bool DropGrid::IsEmpty()
{
	return list.IsEmpty();
}

Vector<String> DropGrid::MakeStringVector(int r) const
{
	Vector<String> v;
	int cnt = value_cols.GetCount();
	int fc = list.GetFixedColumnCount();
	if(cnt > 0)
	{
		for(int i = 0; i < cnt; i++)
		{
			Value val = list.Get(r, value_cols[i]);
			if(IsNull(val))
				continue;
			v.Add(list.GetColumnName(value_cols[i]));
			v.Add(list.GetStdConvertedColumn(value_cols[i] + fc, val));
		}
	}
	return v;
}

Value DropGrid::MakeLongValue(int r, bool columns) const
{
	if(r < 0)
		r = list.GetCursor();

	int cnt = value_cols.GetCount();
	if(cnt > 0)
	{
		String v;
		int fc = list.GetFixedColumnCount();
		for(int i = 0; i < cnt; i++)
		{
			Value val = list.Get(r, value_cols[i]);
			if(IsNull(val))
				continue;
			if(columns)
			{
				v += list.GetColumnName(value_cols[i]);
				v += ' ';
			}
			v += list.GetStdConvertedColumn(value_cols[i] + fc, val);
			v += ' ';
		}
		return v;
	}
	return Null;
}

Value DropGrid::Format0(const Value& q, int rowid) const
{
	int r = rowid >= 0 ? list.FindRow(rowid) : list.Find(q, key_col);
	if(r < 0)
		return Null;

	if(value_cols.GetCount() > 0)
		return RawPickToValue< Vector<String> >(MakeStringVector(r));
	else
		return list.GetConvertedColumn(value_col + list.GetFixedColumnCount(), list.Get(r, value_col));
}

Value DropGrid::Format(const Value& q) const
{
	return Format0(q, -1);
}

void DropGrid::EnableDrop(bool b)
{
	if(IsEnabled() == b)
		return;
	for(int i = 0; i < drop.GetButtonCount(); i++)
		drop.GetButton(i).Enable(b);
	Enable(b);
}

GridCtrl::ItemRect& DropGrid::AddRow(int n, int size)
{
	EnableDrop();
	return list.AddRow(n, size);
}

#define E__Addv0(I)    list.Set(q, I - 1, p##I)
#define E__AddF0(I) \
DropGrid& DropGrid::Add(__List##I(E__Value)) { \
	EnableDrop(true); \
	int q = AddColumns(I); \
	__List##I(E__Addv0); \
	return *this; \
}
__Expand(E__AddF0)

#define E__Addv1(I)    list.Set(q, I - 1, p##I)
#define E__AddF1(I) \
GridCtrl::ItemRect& DropGrid::AddRow(__List##I(E__Value)) { \
	EnableDrop(true); \
	int q = AddColumns(I); \
	GridCtrl::ItemRect& ir = list.AddRow(); \
	__List##I(E__Addv1); \
	return ir; \
}
__Expand(E__AddF1)

END_UPP_NAMESPACE
