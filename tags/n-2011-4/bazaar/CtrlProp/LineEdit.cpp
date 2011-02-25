#include "CtrlPropCommon.h"
#include <CtrlLib/CtrlLib.h>

bool PropSetTabSize(const Value& v, LineEdit& o) { if(!IsNumber(v)) return false; o.TabSize(v); return true; }
bool PropGetTabSize(Value& v, const LineEdit& o) { v = o.GetTabSize(); return true; }
bool PropSetShowTabs(const Value& v, LineEdit& o) { if(!IsNumber(v)) return false; o.ShowTabs(v); return true; }
bool PropGetShowTabs(Value& v, const LineEdit& o) { v = o.IsShowTabs(); return true; }
bool PropSetCutLine(const Value& v, LineEdit& o) { if(!IsNumber(v)) return false; o.WithCutLine(v); return true; }
bool PropGetCutLine(Value& v, const LineEdit& o) { v = o.IsWithCutLine(); return true; }
bool PropSetNoHorzScrollbar(const Value& v, LineEdit& o) { if(!IsNumber(v)) return false; o.NoHorzScrollbar(v); return true; }
bool PropGetNoHorzScrollbar(Value& v, const LineEdit& o) { v = o.IsNoHorzScrollbar(); return true; }

PROPERTIES(LineEdit, TextCtrl)
PROPERTY("tabsize", PropSetTabSize, PropGetTabSize)
PROPERTY("showtabs", PropSetShowTabs, PropGetShowTabs)
PROPERTY("cutline", PropSetCutLine, PropGetCutLine)
PROPERTY("nohorzsb", PropSetNoHorzScrollbar, PropGetNoHorzScrollbar)
END_PROPERTIES

PROPS(Ctrl, LineEdit, TextCtrl)
