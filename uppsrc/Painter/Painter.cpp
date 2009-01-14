#include "Painter.h"

NAMESPACE_UPP

void Painter::OffsetOp(Point p)
{
	Begin();
	Translate(p.x, p.y);
}

void Painter::RectPath(int x, int y, int cx, int cy)
{
	Move(x, y).Line(x + cx - 1, y).Line(x + cx - 1, y + cy - 1).Line(x, y + cy - 1).Close();
}

void Painter::RectPath(const Rect& r)
{
	RectPath(r.left, r.top, r.GetWidth(), r.GetHeight());
}

bool Painter::ClipOp(const Rect& r)
{
	Begin();
	RectPath(r);
	Clip();
	return true;
}

bool Painter::ClipoffOp(const Rect& r)
{
	Begin();
	RectPath(r);
	Clip();
	Translate(r.left, r.top);
	return true;
}

bool Painter::ExcludeClipOp(const Rect& r)
{
	return true;
}

bool Painter::IntersectClipOp(const Rect& r)
{
	RectPath(r);
	Clip();
	return true;
}

Rect Painter::GetClipOp() const
{
	return Rect(0, 0, 0, 0);
}

bool Painter::IsPaintingOp(const Rect& r) const
{
	return true;
}

void Painter::DrawRectOp(int x, int y, int cx, int cy, Color color)
{
	RTIMING("Rect");
	RectPath(x, y, cx, cy);
	Fill(color);
}

void Painter::DrawImageOp(int x, int y, int cx, int cy, const Image& img, const Rect& src, Color color)
{
	RectPath(x, y, cx, cy);
	Fill(img, Translate2D(x, y));
}

void Painter::DrawDataOp(int x, int y, int cx, int cy, const String& data, const char *id)
{
}

void Painter::DrawLineOp(int x1, int y1, int x2, int y2, int width, Color color)
{
	Move(x1, y1);
	Line(x2, y2);
	Stroke(width, color);
}

void Painter::DrawPolyPolylineOp(const Point *vertices, int vertex_count, const int *counts,
                               int count_count, int width, Color color, Color doxor)
{
}

void Painter::DrawPolyPolyPolygonOp(const Point *vertices, int vertex_count,
                                  const int *subpolygon_counts, int scc, const int *disjunct_polygon_counts,
                                  int dpcc, Color color, int width, Color outline, uint64 pattern, Color doxor)
{
}

void Painter::DrawArcOp(const Rect& rc, Point start, Point end, int width, Color color)
{
}

void Painter::DrawEllipseOp(const Rect& r, Color color, int pen, Color pencolor)
{
}

void Painter::DrawTextOp(int x, int y, int angle, const wchar *text, Font font, Color ink, int n, const int *dx)
{
	RTIMING("DrawTextOp");
	Begin();
	EvenOdd(true);
	if(angle)
		Rotate(angle * M_2PI / 36000);
	if(n < 0)
		n = wstrlen(text);
	double *ddx = NULL;
	Buffer<double> h;
	if(dx) {
		h.Alloc(n);
		ddx = h;
		for(int i = 0; i < n; i++)
			ddx[i] = dx[i];
	}
	Text(x, y, text, font, n, ddx);
	Fill(ink);
	End();
}

Painter& Painter::Move(double x, double y)
{
	return Move(x, y, false);
}

Painter& Painter::Line(double x, double y)
{
	return Line(x, y, false);
}

Painter& Painter::Quadratic(double x1, double y1, double x, double y)
{
	return Quadratic(x1, y1, x, y, false);
}

Painter& Painter::Quadratic(double x, double y)
{
	return Quadratic(x, y, false);
}

Painter& Painter::Cubic(double x1, double y1, double x2, double y2, double x, double y)
{
	return Cubic(x1, y1, x2, y2, x, y, false);
}

Painter& Painter::Cubic(double x2, double y2, double x, double y)
{
	return Cubic(x2, y2, x, y, false);
}

Painter& Painter::RelMove(double x, double y)
{
	return Move(x, y, true);
}

Painter& Painter::RelLine(double x, double y)
{
	return Line(x, y, true);
}

Painter& Painter::RelQuadratic(double x1, double y1, double x, double y)
{
	return Quadratic(x1, y1, x, y, true);
}

Painter& Painter::RelQuadratic(double x, double y)
{
	return Quadratic(x, y, true);
}

Painter& Painter::RelCubic(double x1, double y1, double x2, double y2, double x, double y)
{
	return Cubic(x1, y1, x2, y2, x, y, true);
}

Painter& Painter::RelCubic(double x2, double y2, double x, double y)
{
	return Cubic(x2, y2, x, y, true);
}

Matrix2D GetImageLineMatrix(double x1, double y1, double x2, double y2, const Image& image)
{
	Matrix2D m;
	Size sz = image.GetSize();
	m.scale(agg::calc_distance(x1, y1, x2, y2) / sz.cx);
	if(abs(x2 - x1) < abs(y2 - y1) / 1e-6)
		m.rotate(y2 > y1 ? M_PI_2 : -M_PI_2);
	else
		m.rotate(atan((y2 - y1) / (x2 - x1)));
	m.translate(x1, y1);
	return m;
}

Painter& Painter::Fill(const Image& image, double x1, double y1,
                       double x2, double y2, dword flags)
{
	return Fill(image, GetImageLineMatrix(x1, y1, x2, y2, image), flags);
}

Painter& Painter::Stroke(double width, const Image& image, double x1, double y1,
                         double x2, double y2, dword flags)
{
	return Stroke(width, image, GetImageLineMatrix(x1, y1, x2, y2, image), flags);
}

Painter& Painter::Dash(const char *dash, double start)
{
	Vector<double> d;
	CParser p(dash);
	try {
		while(!p.IsEof())
			if(p.Char(':'))
				start = p.ReadDouble();
			else
				d.Add(p.ReadDouble());
	}
	catch(CParser::Error) {}
	Dash(d, start);
	return *this;
}

void Painter::Translate(double x, double y)
{
	Transform(Translate2D(x, y));
}

void Painter::Rotate(double a)
{
	Transform(Rotate2D(a));
}

void Painter::Scale(double scalex, double scaley)
{
	Transform(Scale2D(scalex, scaley));
}

void Painter::Scale(double scale)
{
	Transform(Scale2D(scale));
}

END_UPP_NAMESPACE
