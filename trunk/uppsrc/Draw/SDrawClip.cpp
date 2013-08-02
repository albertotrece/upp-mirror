#include "Draw.h"

NAMESPACE_UPP

void SDraw::Init(const Rect& r)
{
	Cy(r.GetWidth());
	Cloff& c = cloff.Add();
	c.clip.Add(r);
	c.offset = Point(0, 0);
}

void SDraw::BeginOp()
{
	Cloff& c = cloff.Top();
	cloff.Add() <<= c;
}

bool SDraw::ClipOp(const Rect& r)
{
	Cloff& c = cloff.Top();
	Cloff& c1 = cloff.Add();
	c1.clip = Intersection(c.clip, r + c.offset);
	c1.offset = c.offset;
	return c1.clip.GetCount();
}

bool SDraw::ClipoffOp(const Rect& r)
{
	Cloff& c = cloff.Top();
	Cloff& c1 = cloff.Add();
	c1.clip = Intersection(c.clip, r + c.offset);
	c1.offset = c.offset + r.TopLeft();
	return c1.clip.GetCount();
}

bool SDraw::IntersectClipOp(const Rect& r)
{
	Cloff& c = cloff.Top();
	c.clip = Intersection(c.clip, r + c.offset);
	return c.clip.GetCount();
}

bool SDraw::ExcludeClipOp(const Rect& r)
{
	Cloff& c = cloff.Top();
	bool dummy;
	c.clip = Subtract(c.clip, r + c.offset, dummy);
	return c.clip.GetCount();
}

bool SDraw::IsPaintingOp(const Rect& r) const
{
	return true;
}

void SDraw::OffsetOp(Point p)
{
	Cloff& c = cloff.Top();
	Cloff& c1 = cloff.Add();
	c1.clip <<= c.clip;
	c1.offset = c.offset + p;
}

void SDraw::EndOp()
{
	cloff.Drop();
}

END_UPP_NAMESPACE
