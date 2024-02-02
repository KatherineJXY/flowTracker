#include "cm.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

CMSketch::CMSketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
	sprintf(name, "CMSketch");

	if (size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}
	cnt = new int[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

CMSketch::~CMSketch()
{
	if (cnt)
		delete [] cnt;

	if (hash)
		delete [] hash;
}

void 
CMSketch::init()
{
	memset(cnt, 0, size * sizeof(int));
	
	for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(rand() % MAX_PRIME32);
	}
}

void
CMSketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

void
CMSketch::insert(int x, int val)
{

    int i = 0, base = 0;

    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
        int pos = hash[i].run((char*)&x, sizeof(int)) % row_size + base;
        cnt[pos] += val;
	}

}

int
CMSketch::query(int x)
{
    int ans = INT_MAX;

    int i = 0, base = 0;
    for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
        int pos = hash[i].run((char*)&x, sizeof(int)) % row_size + base;
        ans = min(ans, cnt[pos]);
    }

    return ans;
}
