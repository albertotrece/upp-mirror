#include "CtrlPropCommon.h"
#include <CtrlLib/CtrlLib.h>

bool PropSetMin(const Value& v, EditInt& o) { if(!IsNumber(v)) return false; o.MinMax(v, o.GetMax()); return true; }
bool PropGetMin(Value& v, const EditInt& o) { v = o.GetMin(); return true; }
bool PropSetMax(const Value& v, EditInt& o) { if(!IsNumber(v)) return false; o.MinMax(o.GetMin(), v); return true; }
bool PropGetMax(Value& v, const EditInt& o) { v = o.GetMax(); return true; }
bool PropSetNotNull(const Value& v, EditInt& o) { if(!IsNumber(v)) return false; o.NotNull(v); return true; }
bool PropGetNotNull(Value& v, const EditInt& o) { v = o.IsNotNull(); return true; }

PROPERTIES(EditInt, EditField)
PROPERTY("min", PropSetMin, PropGetMin)
PROPERTY("max", PropSetMax, PropGetMax)
PROPERTY("notnull", PropSetNotNull, PropGetNotNull)
END_PROPERTIES

PROPS(Ctrl, EditInt, EditField)
