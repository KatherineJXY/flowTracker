#include "mv-sketch.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

MVSketch::MVSketch(int size, int num_hash):
size(size), num_hash(num_hash)
{
    sprintf(name, "MVSketch");

    if (size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}

    buks = new Buks[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

MVSketch::~MVSketch()
{
    if (buks)
        delete [] buks;
    if (hash)
        delete [] hash;
}

void
MVSketch::init()
{
    memset(buks, 0, size * sizeof(struct Buks));

    for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(rand() % MAX_PRIME32);
	}
}

void
MVSketch::status()
{
    printf("bucket: %d   hash: %d\n", size, num_hash);
}

MVSketch::fiveTuple
MVSketch::segement(string flow)
{
    fiveTuple flowTuple;

    size_t pos = 0;

    if ((pos = flow.find(",")) != string::npos)
    {
        flowTuple.ip_src = flow.substr(0, pos);
        flow.erase(0, pos+1);

        if ((pos = flow.find(",")) != string::npos) 
        {
            flowTuple.ip_dst = flow.substr(0, pos);
            flow.erase(0, pos+1);

            if ((pos = flow.find(",")) != string::npos) 
            {
                flowTuple.l4_src_port = flow.substr(0, pos);
                flow.erase(0, pos+1);

                if ((pos = flow.find(",")) != string::npos) 
                {
                    flowTuple.l4_dst_port = flow.substr(0, pos);
                    flow.erase(0, pos+1);

                    flowTuple.protocol = flow;
                }
            }
        }
    }

    return flowTuple;
}

bool
MVSketch::isEqual(int pos, const fiveTuple& flowTuple)
{
    return(buks[pos].ip_src == flowTuple.ip_src &&
           buks[pos].ip_dst == flowTuple.ip_dst &&
           buks[pos].l4_src_port == flowTuple.l4_src_port &&
           buks[pos].l4_dst_port == flowTuple.l4_dst_port &&
           buks[pos].protocol == flowTuple.protocol);
}

void
MVSketch::recordPromotion(int pos, const fiveTuple& flowTuple)
{
    buks[pos].ip_src = flowTuple.ip_src;
    buks[pos].ip_dst = flowTuple.ip_dst;
    buks[pos].l4_src_port = flowTuple.l4_src_port;
    buks[pos].l4_dst_port = flowTuple.l4_dst_port;
    buks[pos].protocol = flowTuple.protocol;
}

void
MVSketch::insert(string flow, int val)
{
    fiveTuple flowTuple = segement(flow);

    int i = 0, base = 0;
    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        buks[pos].total_sum += val;
        if (isEqual(pos, flowTuple))
        {
            buks[pos].indicator += val;
        }
        else 
        {
            buks[pos].indicator -= val;
            if (buks[pos].indicator < 0)
            {
                recordPromotion(pos, flowTuple);
                buks[pos].indicator = -buks[pos].indicator;
            }
        }
	}
}

int
MVSketch::query(string flow)
{
    int ans = INT_MAX;
    fiveTuple flowTuple = segement(flow);

    int i = 0, base = 0;
    for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        if (isEqual(pos, flowTuple))
        {
            ans = min(ans, ((buks[pos].total_sum + buks[pos].indicator)/2));
        }
        else
        {
            ans = min(ans, ((buks[pos].total_sum - buks[pos].indicator)/2));
        }
    }

    return ans;
}