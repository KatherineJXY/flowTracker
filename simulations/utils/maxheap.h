#ifndef MAXHEAP_HEADER
#define MAXHEAP_HEADER

#include "sketch.h"
#include <set>
using std::set;

class MaxHeap
{
private:
	struct comp
	{
		template<typename T>
		bool operator()(const T &l, const T &r) const
		{
			if (l.second != r.second) {
				return l.second > r.second;
			}

			return l.first > r.first;
		}
	};

	set<PSI, comp> data;
	int max_size;
public:
	MaxHeap() { max_size = 300; };
	MaxHeap(int x) : max_size(x) {};
	~MaxHeap() {};
	int size() { return data.size(); }
	void init() { data.clear();}
	void set_maxsize(int x) { max_size = x; }
	int get_maxsize() { return max_size; }
	bool insert(string flow, int val); // try to push, return 1 if success.
	PSI top() { return data.size()==0? mp("", 0): (*data.begin()); }
	set<string> heavy_hitter(int threshold);	// return heavy hitter of the heap
	set<string> topk(int k);	// return topk of the heap
};

#endif // MAXHEAP_HEADER