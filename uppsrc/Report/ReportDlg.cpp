#include "Report.h"

#define LLOG(x) // LOG(x)

#ifdef PLATFORM_WIN32
#include <commdlg.h>
#pragma  comment(lib, "comdlg32.lib")
#endif

bool Print(Report& r, int i, const char *_name) {
#ifdef PLATFORM_WIN32
	PRINTDLG dlg;
	memset(&dlg, 0, sizeof(dlg));
	dlg.lStructSize = sizeof(dlg);
	dlg.hwndOwner = GetActiveWindow();
	dlg.Flags = PD_ALLPAGES|PD_DISABLEPRINTTOFILE|PD_NOSELECTION|PD_RETURNDC|
		        PD_HIDEPRINTTOFILE|PD_RETURNDEFAULT;
	if(!PrintDlg(&dlg)) return false;
	Size pgsz = r.GetPage(0).GetSize();
	if(dlg.hDevMode) {
		DEVMODE *pDevMode = (DEVMODE*)::GlobalLock(dlg.hDevMode);
		pDevMode->dmOrientation = pgsz.cx > pgsz.cy ? DMORIENT_LANDSCAPE
		                                            : DMORIENT_PORTRAIT;
		::GlobalUnlock(dlg.hDevMode);
	}
	dlg.Flags = PD_ALLPAGES|PD_DISABLEPRINTTOFILE|PD_NOSELECTION|PD_RETURNDC|
		        PD_HIDEPRINTTOFILE;
	dlg.nFromPage = i + 1;
	dlg.nToPage = i + 1;
	dlg.nMinPage = 1;
	dlg.nMaxPage = r.GetCount();
	if(PrintDlg(&dlg)) {
		PrintDraw w(dlg.hDC, _name);
		if(!(dlg.Flags & PD_PAGENUMS)) {
			dlg.nFromPage = dlg.nMinPage;
			dlg.nToPage = dlg.nMaxPage;
		}
		Size sz = w.GetPageMMs();
		int x = (6000 * sz.cx / 254 - pgsz.cx) / 2;
		int y = (6000 * sz.cy / 254 - pgsz.cy) / 2;
		for(int c = 0; c < ((dlg.Flags & PD_COLLATE) ? dlg.nCopies : 1); c++)
			for(i = dlg.nFromPage - 1; i <= dlg.nToPage - 1; i++)
				for(int c = 0; c < ((dlg.Flags & PD_COLLATE) ? 1 : dlg.nCopies); c++) {
					LLOG("Printing Page " << i << " gdi: " << GdiGetFreeSpace());
					Drawing iw = r.GetPage(i);
					Size sz = iw.GetSize();
					w.StartPage();
					w.DrawDrawing(x, y, sz.cx, sz.cy, iw);
					w.EndPage();
				}
	}
	else
		dlg.nToPage = (WORD)-1;
	::GlobalFree(dlg.hDevMode);
	::GlobalFree(dlg.hDevNames);
	return dlg.nFromPage - 1 == 0 && dlg.nToPage == r.GetCount();
#endif
	return false;
}

void ReportView::MouseWheel(Point, int zdelta, dword) {
	sb.Wheel(zdelta);
}

ReportView::ReportView() {
	report = NULL;
	vsize = 0;
	pagesize.cx = pagesize.cy = 0;
	sb.WhenScroll = callback(this, &ReportView::Sb);
	sb.SetLine(16);
	AddFrame(sb);
	SetFrame(FieldFrame());
	numbers = false;
	pages = PG1;
}

void ReportView::Set(Report& p) {
	Init();
	report = &p;
	Refresh();
	Layout();
}

Size ReportView::GetReportSize()
{
	return report->GetPage(0).GetSize();
}

bool ReportView::Key(dword key, int) {
	return sb.VertKey(key);
}

void ReportView::LeftDown(Point p, dword) {
	if(!report || pagesize.cy == 0 || pagesize.cx == 0) return;
	int pg = (sb + p.y) / pagesize.cy * pvn + min(pvn - 1, p.x / pagesize.cx);
	WhenGoPage();
	Layout();
	sb = pg * pagesize.cy;
}

void ReportView::Sb() {
	Refresh();
	WhenAction();
}

void ReportView::Init() {
	for(int i = 0; i < 64; i++) {
		page[i].Clear();
		pagei[i] = -1;
	}
}

void ReportView::Layout() {
	Size sz = GetSize();
	if(report && report->GetCount()) {
		ASSERT(pages >= PG1 && pages <= PG16);
		pvn = 1 << pages;
		static int h[] = { 1, 3, 15, 63 };
		pm = h[pages];
		Size sr = report->GetPage(0).GetSize();
		pagesize.cx = sz.cx / pvn;
		pagesize.cy = sr.cy * pagesize.cx / sr.cx;
		vsize = (report->GetCount() + pvn - 1) / pvn * pagesize.cy;
		sb.SetPage(sz.cy);
		sb.SetTotal(vsize);
		Init();
		Refresh();
	}
	else {
		vsize = 0;
		sb.Set(0, 0, 0);
		return;
	}
}

Image ReportView::GetPage(int i) {
	ASSERT(report);
	int ii = i & pm;
	if(pagei[ii] != i) {
		pagei[ii] = i;
	#ifdef NEWIMAGE
		Size sz = Size(max(pagesize.cx - 2, 1), max(pagesize.cy - 2, 1));
		ImageDraw iw(sz);
		iw.DrawRect(sz, White);
		iw.DrawDrawing(0, 0, sz.cx, sz.cy, report->GetPage(i));
		page[ii] = iw;
	#else
		page[ii] = Image(pagesize.cx - 2, pagesize.cy - 2, report->GetPage(i), White);
	#endif
	}
	return page[ii];
}

void ReportView::ScrollInto(int toppage, int top, int bottompage, int bottom) {
	sb.ScrollInto(toppage * pagesize.cy + top * pagesize.cy / GetReportSize().cy,
	              (bottompage - toppage) * pagesize.cy +
	                (bottom - top) * pagesize.cy / GetReportSize().cy);
}

void ReportView::Paint(Draw& w) {
	Size sz = GetSize();
	if(sz.cx <= 0 || sz.cy <= 0) return;
	if(!vsize || !report) {
		w.DrawRect(0, 0, sz.cx, sz.cy, SGray);
		return;
	}
	int i = sb / pagesize.cy;
	int y = i * pagesize.cy - sb;
	i *= pvn;
	while(y < sz.cy) {
		int x = 0;
		for(int j = pvn; j--;) {
			if(i < report->GetCount()) {
				w.DrawImage(x + 1, y + 1, GetPage(i));
				if(j == 0) {
					w.DrawRect(x + pagesize.cx - 1, y, sz.cx - x - pagesize.cx, pagesize.cy, White);
					DrawFrame(w, x, y, sz.cx - x, pagesize.cy, White, LtGray);
				}
				else
					DrawFrame(w, x, y, pagesize.cx, pagesize.cy, White, LtGray);
				if(numbers) {
					String n = Format("%d", i + 1);
					Size tsz = w.GetTextSize(n);
					tsz += Size(8, 4);
					int tx = x + pagesize.cx - tsz.cx;
					DrawFrame(w, tx, y, tsz.cx, tsz.cy, Black, Black);
					w.DrawRect(tx + 1, y + 1, tsz.cx - 2, tsz.cy - 2, Yellow);
					w.DrawText(tx + 4, y + 2, n);
				}
			}
			else {
				w.DrawRect(x, y, sz.cx - x, pagesize.cy, Gray);
				break;
			}
			x += pagesize.cx;
			i++;
		}
		y += pagesize.cy;
	}
}

ReportWindow::ReportWindow()
{
	CtrlLayoutCancel(*this, "");
	sw <<= ReportView::PG1;
	sw <<= THISBACK(Pages);
	numbers <<= THISBACK(Numbers);
	pg.WhenGoPage = THISBACK(GoPage);
	lbl.SetFont(Arial(20).Italic());
	lbl.SetAlign(ALIGN_CENTER);
	pg.WhenAction = Breaker(-1);
	Sizeable();
	MaximizeBox();
	Icon(CtrlImg::smallreporticon());
	SetButton(0, t_("&Print"), 999995);
	pdf.Show(GetDrawingToPdfFn());
	pdf <<= THISBACK(Pdf);
}

String Pdf(Report& report)
{
	return GetDrawingToPdfFn() && report.GetCount() ?
	      (*GetDrawingToPdfFn())(report.GetPages(), report.GetPage(0).GetSize(), 200)
	      : String();
}

void ReportWindow::Pdf()
{
	static FileSel fs;
	static bool b;
	if(!b) {
		fs.Type("PDF file", "*.pdf");
		fs.AllFilesType();
	}
	if(!fs.ExecuteSaveAs("Output PDF file"))
		return;
	SaveFile(~fs, ::Pdf(*report));
}

void ReportWindow::SetButton(int i, const char *label, int id)
{
	LogPos p = cancel.GetPos();
	Button& b = button.At(i);
	b.SetLabel(label);
	b.RightPos(p.x.GetA(), p.x.GetB()).BottomPos(p.y.GetA() + (p.y.GetB() + 6) * (i + 1), p.y.GetB());
	b <<= Breaker(id);
	for(int i = 0; i < button.GetCount(); i++)
		button[i].Remove();
	for(int i = 0; i < button.GetCount(); i++)
		AddChildBefore(&button[i], &cancel);
}

int ReportWindow::Perform(Report& report, int zoom, const char *caption)
{
	this->report = &report;
//	if(report.dortf)
//		WriteClipboard(GetClipboardFormatCode("Rich text format"), report.rtf.Get());
	pg.Set(report);
	Size sz;
	Rect area = Ctrl::GetWorkArea();
	sz = area.Size() * 4 * abs(zoom) / 500;
	SetRect(sz);

	ActiveFocus(pg);
	Title(caption);
	Open();
	int n = -1;
	for(;;) {
		int nn = pg.GetFirst() + 1;
		if(n != nn)
			lbl = Format("%d / %d", n = nn, report.GetCount());
		int c = Run();
		switch(c) {
		case IDCANCEL:
			return false;
		case 999995:
			return Print(report, pg.GetFirst(), caption);
		}
		if(c > 0)
			return c;
	}
}

bool Perform(Report& r, const char *caption)
{
	return ReportWindow().Perform(r, 100, caption);
}
