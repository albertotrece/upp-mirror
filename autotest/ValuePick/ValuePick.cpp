#include <Core/Core.h>

#define N 500000

using namespace Upp;

#define TEST(fn, s) { \
	String r = AsString(fn); \
	LOG("TEST(" << #fn << ", " << AsCString(r) << ");"); \
	if(s) ASSERT((char *)s == r); \
}

CONSOLE_APP_MAIN
{
	StdLogSetup(LOG_COUT|LOG_FILE);

	{
		Vector<Value> v;
		v << 5 << 1 << 3;
		ValueArray va = pick(v);
		ASSERT(v.IsPicked());
		TEST(va, "[5, 1, 3]");
		Vector<Value> v2 = va.Pick();
		TEST(v2, "[5, 1, 3]");
		Sort(v2);
		va = pick(v2);
		ASSERT(v2.IsPicked());
		TEST(va, "[1, 3, 5]");
	}
	{
		VectorMap<Value, Value> x;
		x(1, "kamarade");
		x(2, "ahoj");
		
		ValueMap m = pick(x);
		
		TEST(m, "{ 1: kamarade, 2: ahoj }");
		
		x = m.Pick();
	
		TEST(x, "{1: kamarade, 2: ahoj}");
		
		SortByValue(x);
	
		TEST(x, "{2: ahoj, 1: kamarade}");
		
		m = pick(x);
	
		TEST(m, "{ 2: ahoj, 1: kamarade }");
	}
	{
		ValueMap m;
		m("1", 1)("2", 2);
		ValueMap m1 = m;
		ValueMap m2 = m;
		m1.RemoveKey("1");
		m1.RemoveKey("3");
		m2.RemoveKey("2");
		
		TEST(m, "{ 1: 1, 2: 2 }");
		TEST(m1, "{ 2: 2 }");
		TEST(m2, "{ 1: 1 }");
	}	
	
	LOG("============== OK");
}
