#include "sds.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

SimpleDelaySketch::SimpleDelaySketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
    sprintf(name, "SimpleDelaySketch");

	if (size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}
    sds_buks = new sdsBuks[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

SimpleDelaySketch::~SimpleDelaySketch()
{
    if (sds_buks)
        delete [] sds_buks;
    if (hash)
        delete [] hash;
}

void
SimpleDelaySketch::init()
{
    memset(sds_buks, 0, size * sizeof(struct sdsBuks));
	
	for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(rand() % MAX_PRIME32);
	}
}

void
SimpleDelaySketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

void
SimpleDelaySketch::insert(string flow, int val)
{

    int i = 0, base = 0;

    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        sds_buks[pos].count ++;
        sds_buks[pos].delay_sum += val;
	}

}

double
SimpleDelaySketch::query_average(string flow)
{
    double empty_ans = 0.0;
    int min_count = INT_MAX;
    int min_sum = INT_MAX;

    int i = 0, base = 0;
    for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        min_count = min(min_count, sds_buks[pos].count);
        min_sum = min(min_sum, sds_buks[pos].delay_sum);
    }

    if (min_count != 0)
        return double(min_sum/min_count);
    
    return empty_ans;
}