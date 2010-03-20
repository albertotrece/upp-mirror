#include <Report/Report.h>

GUI_APP_MAIN
{
	Report r;
	r.Header("[A2> Page $$P");
	r << "This is some [* QTF text";
	r << "&&Let's report some table:";
	String tab;
	tab << "{{1:1:1:1:1:1:2 A:: B:: C:: D:: E:: F:: G (wider)";
	for(int row = 0; row < 20; row++)
		for(int column = 0; column < 7; column++)
			tab << ":: " << row << ":" << column;
	r << tab;
	r.NewPage();
	int y = r.GetY();
	r.DrawEllipse(300, y + 50, 3000, 2000, LtRed, 20);
	r.DrawText(500, y + 50 + 950, "Some free drawing in this page!", Arial(100));
	Vector<Point> poly;
	poly << Point(200,200) << Point(700,200) << Point(700,500) << Point(200,500) << Point(200,200);
	r.DrawPolygon(poly, Cyan, 2, Red, 0, Null);
	r.SetY(y + 50 + 3000);
	r << "More of [/ QTF] [C6* text";
	Perform(r);
}
