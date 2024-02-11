#include "Apnet_sketch.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

ApnetSketch::ApnetSketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
    sprintf(name, "ApnetSketch");

	if (size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}
	
    bulks = new Bulks[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

ApnetSketch::~ApnetSketch()
{
    if (bulks)
        delete [] bulks;
    if (hash)
        delete [] hash;
}

void 
ApnetSketch::init()
{
	memset(bulks, 0, size * sizeof(struct Bulks));
	
	for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(rand() % MAX_PRIME32);
	}
}

void
ApnetSketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

ApnetSketch::fiveTuple
ApnetSketch::segement(string flow)
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
ApnetSketch::isEqual(int pos, const fiveTuple& flowTuple)
{
     
    return(bulks[pos].ip_src == flowTuple.ip_src &&
           bulks[pos].ip_dst == flowTuple.ip_dst &&
           bulks[pos].l4_src_port == flowTuple.l4_src_port &&
           bulks[pos].l4_dst_port == flowTuple.l4_dst_port &&
           bulks[pos].protocol == flowTuple.protocol);
}

bool
ApnetSketch::isEmpty(int pos, const fiveTuple& flowTuple)
{
    if (bulks[pos].ip_src.empty() &&
        bulks[pos].ip_dst.empty() &&
        bulks[pos].l4_src_port.empty() &&
        bulks[pos].l4_dst_port.empty() &&
        bulks[pos].protocol.empty() &&
        bulks[pos].max_count == 0)
    {
        bulks[pos].ip_src = flowTuple.ip_src;
        bulks[pos].ip_dst = flowTuple.ip_dst;
        bulks[pos].l4_src_port = flowTuple.l4_src_port;
        bulks[pos].l4_dst_port = flowTuple.l4_dst_port;
        bulks[pos].protocol = flowTuple.protocol;
        return true;
    }

    return false;
}

void
ApnetSketch::recordPromotion(int pos, const fiveTuple& flowTuple, int max_count)
{
    bulks[pos].ip_src = flowTuple.ip_src;
    bulks[pos].ip_dst = flowTuple.ip_dst;
    bulks[pos].l4_src_port = flowTuple.l4_src_port;
    bulks[pos].l4_dst_port = flowTuple.l4_dst_port;
    bulks[pos].protocol = flowTuple.protocol;
    bulks[pos].max_count = max_count;
}

void
ApnetSketch::insert(string flow, int val)
{
    fiveTuple flowTuple = segement(flow);
    int i = 0, base = 0;
    int max_record = INT_MIN;
    int sav_pos;
    bool replace = true;

    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        if (bulks[pos].max_count > max_record)
        {
            max_record = bulks[pos].max_count;
            sav_pos = pos;
        }

        if (isEmpty(pos, flowTuple) || isEqual(pos, flowTuple))
        {
            bulks[pos].max_count = max(bulks[pos].max_count, val);
            replace = false;
        }
	}

    if (replace)
    {
        recordPromotion(sav_pos, flowTuple, val);
    }
}

int
ApnetSketch::query_max(string flow)
{
    int empty_ans = 0;
    fiveTuple flowTuple = segement(flow);

    int i = 0, base = 0;
    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
        int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        if (isEqual(pos, flowTuple))
        {
            return bulks[pos].max_count;
        }
    }

    return empty_ans;
}