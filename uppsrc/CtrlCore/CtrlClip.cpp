#include "CtrlCore.h"

NAMESPACE_UPP

void Ctrl::DragEnter(Point p, PasteClip& d)         {}
void Ctrl::DragAndDrop(Point p, PasteClip& d)       {}
void Ctrl::ChildDragAndDrop(Point p, PasteClip& d)  {}
void Ctrl::DragRepeat(Point p) {}
void Ctrl::DragLeave() {}

String Ctrl::GetDropData(const String& fmt) const
{
	return GetSelectionData(fmt);
}

String Ctrl::GetSelectionData(const String& fmt) const
{
	return Null;
}

#ifdef PLATFORM_WIN32
bool   Has(UDropTarget *dt, const char *fmt);
String Get(UDropTarget *dt, const char *fmt);

bool PasteClip::IsAvailable(const char *fmt) const
{
	return dt ? UPP::Has(dt, fmt) : IsClipboardAvailable(fmt);
}

String PasteClip::Get(const char *fmt) const
{
	return dt ? UPP::Get(dt, fmt) : ReadClipboard(fmt);
}
#endif

#ifdef PLATFORM_X11
bool   XDnDHas(const char *fmt);
String XDnDGet(const char *fmt);

bool PasteClip::IsAvailable(const char *fmt) const
{
	return dnd ? XDnDHas(fmt) : IsClipboardAvailable(fmt);
}

String PasteClip::Get(const char *fmt) const
{
	return dnd ? XDnDGet(fmt) : ReadClipboard(fmt);
}
#endif

bool PasteClip::Accept()
{
	accepted = true;
	return paste;
}

bool   PasteClip::Accept(const char *_fmt)
{
	Vector<String> f = Split(_fmt, ';');
	for(int i = 0; i < f.GetCount(); i++) {
		if(IsAccepted() && fmt == _fmt)
			return paste;
		if(IsAvailable(f[i])) {
			accepted = true;
			if(paste) {
				fmt = f[i];
				data = Get(f[i]);
				return true;
			}
			break;
		}
	}
	return false;
}

PasteClip::PasteClip()
{
	paste = true;
	accepted = false;
#ifdef PLATFORM_WIN32
	dt = NULL;
#else
	dnd = false;
#endif
}

PasteClip& Ctrl::Clipboard()
{
	static PasteClip d;
	d.fmt.Clear();
	return d;
}

String ClipFmtsText()
{
	return "wtext;text";
}

bool AcceptText(PasteClip& clip)
{
	return clip.Accept("wtext;text");
}

void AddTextClip(VectorMap<String, String>& data, const String& text)
{
	data.GetAdd("text", GetTextClip(text, "text"));
	data.GetAdd("wtext", GetTextClip(text, "wtext"));
}

void AddTextClip(VectorMap<String, String>& data, const WString& text)
{
	data.GetAdd("text", GetTextClip(text, "text"));
	data.GetAdd("wtext", GetTextClip(text, "wtext"));
}

int Ctrl::DoDragAndDrop(const char *fmts, const Image& sample, dword actions)
{
	VectorMap<String, String> dummy;
	return DoDragAndDrop(fmts, sample, actions, dummy);
}

int Ctrl::DoDragAndDrop(const VectorMap<String, String>& data, const Image& sample, dword actions)
{
	return DoDragAndDrop("", sample, actions, data);
}


Uuid        sDndUuid;
const void *sInternalPtr;

String GetInternalDropId__(const char *type, const char *id)
{
	return "U++ Internal clip:" + AsString(sDndUuid) + '-' + type + '-' + id;
}

void NewInternalDrop__(const void *ptr)
{
	sDndUuid = Uuid::Create();
	sInternalPtr = ptr;
}

const void *GetInternalDropPtr__()
{
	return sInternalPtr;
}

Ctrl *Ctrl::FindCtrl(Ctrl *ctrl, Point& p)
{
	for(;;) {
		Ctrl *c = ctrl->ChildFromPoint(p);
		if(!c) break;
		ctrl = c;
	}
	return ctrl;
}

END_UPP_NAMESPACE
