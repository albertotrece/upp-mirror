#include <Core/Core.h>

using namespace Upp;

#define NOOUTPUT

int main(int argc, const char *argv[])
{
	int n;
	VectorMap<String, int> map;
	Cout() << "   lines   words   bytes file\n";
	int total_lines = 0;
	int total_words = 0;
	int total_bytes = 0;
	for(int i = 1; i < argc; i++) {
		String f = LoadFile(argv[i]);
		int lines = 0;
		int words = 0;
		const char *q = f;
		for(;;) {
			int c = *q;
			if(IsAlpha(c)) {
				const char *b = q++;
				while(IsAlNum(*q)) q++;
				map.GetAdd(String(b, q), 0)++;
				words++;
			}
			else {
				if(!c) break;
				if(c == '\n')
					++lines;
				q++;
			}
		}
		Cout() << Format("%8d%8d%8d %s\n", lines, words, f.GetCount(), argv[i]);
		total_lines += lines;
		total_words += words;
		total_bytes += f.GetCount();
	}
	Vector<int> order = GetSortOrder(map.GetKeys());
#ifndef NOOUTPUT
	Cout() << Format("--------------------------------------%8d%8d%8d total\n", total_lines, total_words, total_bytes);

	for(int i = 0; i < order.GetCount(); i++)
		Cout() << map.GetKey(order[i]) << ": " << map[order[i]] << '\n';
#endif
	return 0;
}
