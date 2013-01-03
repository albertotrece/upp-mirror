#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct DndTest : public TopWindow {
	virtual void Paint(Draw &w);
	virtual void DragAndDrop(Point p, PasteClip& d);
	virtual bool Key(dword key, int count);
	Vector<String> files;

	DndTest();
};

void DndTest::Paint(Draw &w)
{
	w.DrawRect(GetSize(), SColorPaper());
	if(files.GetCount())
		for(int i = 0; i < files.GetCount(); i++)
			w.DrawText(2, 2 + i * Draw::GetStdFontCy(), files[i]);
	else
		w.DrawText(2, 2, "None");
}

void DndTest::DragAndDrop(Point p, PasteClip& d)
{
	if (AcceptFiles(d)) {
		files = GetFiles(d);
		Refresh();
	}
}

bool DndTest::Key(dword key, int count)
{
	if(key == K_CTRL_V) {
		files = GetFiles(Ctrl::Clipboard());
		Refresh();
		return true;
	}
	return false;
}

DndTest::DndTest()
{
	Title("I need files!");
}

GUI_APP_MAIN
{
	DndTest().Run();
}
