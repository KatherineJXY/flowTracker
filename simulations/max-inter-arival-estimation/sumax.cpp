#include "sumax.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

SuMax::SuMax(int size, int num_hash):
size(size), num_hash(num_hash)
{
    sprintf(name, "SuMax");

	if (size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}
	max_cnt = new int[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

SuMax::~SuMax()
{
    if (max_cnt)
        delete [] max_cnt;
    if (hash)
        delete [] hash;
}

void 
SuMax::init()
{
	memset(max_cnt, 0, size * sizeof(int));
	
	for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(rand() % MAX_PRIME32);
	}
}

void
SuMax::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

void
SuMax::insert(string flow, int val)
{

    int i = 0, base = 0;

    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        max_cnt[pos] = max(max_cnt[pos], val);
	}

}

int
SuMax::query_max(string flow)
{
    int ans = INT_MAX;

    int i = 0, base = 0;
    for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        ans = min(ans, max_cnt[pos]);
    }

    return ans;
}
