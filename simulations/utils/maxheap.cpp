#include "maxheap.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <climits>
#include <cstdint>
#include <map>
using std::map;

bool
MaxHeap::insert(string flow, int val)
{	
	if (data.size() < max_size)
	{
		data.insert(mp(flow, val));
		return true;
	}

	set<PSI>::iterator min_ele = --data.end();
	if ((*min_ele).sc < val)
	{
		data.erase(min_ele);
		data.insert(mp(flow, val));
		return true;
	}

	return false;
}

set<string>
MaxHeap::topk(int k)
{
	set<PSI>::iterator it = data.begin();
	set<string> ans;

	if (data.size() < k)
		k = (int)data.size();
	while(k--)
	{
		ans.insert((*it).first);
		it++;
	}

	return ans;
}

set<string>
MaxHeap::heavy_hitter(int threshold)
{
	set<PSI>::iterator it = data.begin();
	set<string> ans;

	while(it != data.end())
	{
		if ((*it).second < threshold)	break;
		ans.insert((*it).first);
		it ++;
	}
	return ans;
}