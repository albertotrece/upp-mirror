#include "RichText.h"

#ifdef NEWIMAGE

struct RichImage : public RichObjectType {
	virtual String GetTypeName(const Value& v) const;
	virtual Size   GetPhysicalSize(const Value& data) const;
	virtual Size   GetPixelSize(const Value& data) const;
	virtual void   Paint(const Value& data, Draw& w, Size sz) const;
	virtual Value  ReadClipboard() const;
	virtual void   WriteClipboard(const Value& v) const;

	typedef RichImage CLASSNAME;
};

String RichImage::GetTypeName(const Value& v) const
{
	return "image";
}

Value RichImage::ReadClipboard() const
{
	Image img = ReadClipboardImage();
	return img ? StoreImageAsString(img) : String();
}

void  RichImage::WriteClipboard(const Value& v) const
{
	WriteClipboardImage(LoadImageFromString(v));
}

Size   RichImage::GetPixelSize(const Value& data) const
{
	return GetImageStringSize(data);
}

Size   RichImage::GetPhysicalSize(const Value& data) const
{
	return GetImageStringDots(data);
}

void   RichImage::Paint(const Value& data, Draw& w, Size sz) const
{
	Image x = LoadImageFromString(data);
	Size outsz(min(sz.cx, 4 * x.GetWidth()), min(sz.cy, 4 * x.GetHeight()));
	w.DrawImage(0, 0, Rescale(x, outsz.cx, outsz.cy));
}

INITBLOCK {
	RichObject::Register("image", &Single<RichImage>());
};

struct RichPNG : public RichObjectType {
	virtual String GetTypeName(const Value& v) const;
	virtual Value  Read(const String& s) const;
	virtual String Write(const Value& v) const;
	virtual Size   GetPhysicalSize(const Value& data) const;
	virtual Size   GetPixelSize(const Value& data) const;
	virtual void   Paint(const Value& data, Draw& w, Size sz) const;
};

String RichPNG::GetTypeName(const Value& v) const
{
	return IsString(v) ? "PNG" : "image";
}

Value RichPNG::Read(const String& s) const
{
	Image img = StreamRaster::LoadStringAny(s);
	if(img)
		return img;
	return s;
}

String RichPNG::Write(const Value& v) const
{
	if(IsString(v))
		return v;
	return StoreImageAsString(v);
}

Size RichPNG::GetPhysicalSize(const Value& data) const
{
	if(IsString(data))
		return Size(0, 0);
	return Image(data).GetDots();
}

Size RichPNG::GetPixelSize(const Value& data) const
{
	if(IsString(data))
		return Size(0, 0);
	return Image(data).GetDots();
}

void RichPNG::Paint(const Value& data, Draw& w, Size sz) const
{
	if(IsString(data)) {
		w.DrawRect(sz, SColorFace());
		DrawFrame(w, sz, SColorText());
		w.DrawText(2, 2, "PNG plugin missing!");
		return;
	}
	Image x = Image(data);
	Size outsz(min(sz.cx, 4 * x.GetWidth()), min(sz.cy, 4 * x.GetHeight()));
	w.DrawImage(0, 0, Rescale(x, outsz.cx, outsz.cy));
}

INITBLOCK {
	RichObject::Register("PNG", &Single<RichPNG>());
};

#endif
