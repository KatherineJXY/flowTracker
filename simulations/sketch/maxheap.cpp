#include "maxheap.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <climits>
#include <cstdint>
#include <map>
using std::map;

bool
MaxHeap::insert(int x, int v)
{	
	if (data.size() < max_size)
	{
		data.insert(mp(x, v));
		return true;
	}

	set<PII>::iterator min_ele = data.end();
	if ((*min_ele).sc < v)
	{
		data.erase(min_ele);
		data.insert(mp(x, v));
		return true;
	}

	return false;
}

set<int>
MaxHeap::topk(int k)
{
	set<PII>::iterator it = data.begin();
	set<int> ans;

	if (data.size() < k)
		k = (int)data.size();
	while(k--)
	{
		ans.insert((*it).first);
		it++;
	}

	return ans;
}

set<int>
MaxHeap::heavy_hitter(int threshold)
{
	set<PII>::iterator it = data.begin();
	set<int> ans;

	while(it != data.end())
	{
		if ((*it).second < threshold)	break;
		ans.insert((*it).first);
		it ++;
	}
	return ans;
}