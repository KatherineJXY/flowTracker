#include "hashflow.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

HashFlow::HashFlow(int main_table_size, int ancillary_table_size, int num_hash) :
main_table_size(main_table_size), ancillary_table_size(ancillary_table_size), num_hash(num_hash)
{
    sprintf(name, "HashFlow");
    if (main_table_size <= 0 || ancillary_table_size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}

	main_table = new mainTableRecords[main_table_size];
    ancillary_table = new ancillaryTableRecords[ancillary_table_size];
	main_table_hashes = new BOBHash32[num_hash];
    ancillary_table_hash = new BOBHash32();
}

HashFlow::~HashFlow()
{
    if (main_table)
        delete [] main_table;
    
    if (ancillary_table)
        delete [] ancillary_table;

    if (main_table_hashes)
        delete [] main_table_hashes;
    
    if (ancillary_table_hash)
        delete ancillary_table_hash;
}

void
HashFlow::init()
{
    memset(main_table, 0, main_table_size * sizeof(struct mainTableRecords));
    memset(ancillary_table, 0, ancillary_table_size * sizeof(struct ancillaryTableRecords));

    ancillary_table_hash->initialize(rand() % MAX_PRIME32);

    for (int i = 0; i < num_hash; i++) 
    {
        main_table_hashes[i].initialize(rand() % MAX_PRIME32);
    }
}

void
HashFlow::status()
{
    printf("Main table: counter arrays %d   hash:%d\n", main_table_size, num_hash);
    printf("Ancillary table: bucket %d\n", ancillary_table_size);
}

HashFlow::fiveTuple
HashFlow::segement(string flow)
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
HashFlow::areEqualorEmpty(int pos, const fiveTuple& flowTuple)
{
    if (main_table[pos].ip_src.empty() &&
        main_table[pos].ip_dst.empty() &&
        main_table[pos].l4_src_port.empty() &&
        main_table[pos].l4_dst_port.empty() &&
        main_table[pos].protocol.empty() &&
        main_table[pos].count == 0)
    {
        main_table[pos].ip_src = flowTuple.ip_src;
        main_table[pos].ip_dst = flowTuple.ip_dst;
        main_table[pos].l4_src_port = flowTuple.l4_src_port;
        main_table[pos].l4_dst_port = flowTuple.l4_dst_port;
        main_table[pos].protocol = flowTuple.protocol;
        return true;
    } 
    else if (main_table[pos].ip_src == flowTuple.ip_src &&
            main_table[pos].ip_dst == flowTuple.ip_dst &&
            main_table[pos].l4_src_port == flowTuple.l4_src_port &&
            main_table[pos].l4_dst_port == flowTuple.l4_dst_port &&
            main_table[pos].protocol == flowTuple.protocol)
    {
        return true;
    }
    
    return false;
}

void
HashFlow::recordPromotion(int pos, const fiveTuple& flowTuple, int count)
{
    main_table[pos].ip_src = flowTuple.ip_src;
    main_table[pos].ip_dst = flowTuple.ip_dst;
    main_table[pos].l4_src_port = flowTuple.l4_src_port;
    main_table[pos].l4_dst_port = flowTuple.l4_dst_port;
    main_table[pos].protocol = flowTuple.protocol;
    main_table[pos].count = count;
}

void
HashFlow::insert(string flow, int val)
{
    fiveTuple flowTuple = segement(flow);
    bool success = false;
    int main_table_pos, minimum_pos;
    int minimum_count = INT32_MAX;

    for (int i = 0; i < num_hash; ++i)
    {
        main_table_pos = main_table_hashes[i].run(flow.c_str(), flow.length()) % main_table_size;
        
        if (areEqualorEmpty(main_table_pos, flowTuple))
        {
            main_table[main_table_pos].count += val;
            success = true;
            break;
        }

        if (main_table[main_table_pos].count < minimum_count)
        {
            minimum_count = main_table[main_table_pos].count;
            minimum_pos = main_table_pos;
        }
    }

    if (!success)
    {
        int pos = ancillary_table_hash->run(flow.c_str(), flow.length()) % ancillary_table_size;

        if (ancillary_table[pos].digest == main_table_pos) 
        {
            ancillary_table[pos].count += val;
        } 
        else 
        {
            ancillary_table[pos].digest = main_table_pos;
            ancillary_table[pos].count = val;
        }

        // record promotion
        if (ancillary_table[pos].count >= minimum_count) 
        {
            recordPromotion(minimum_pos, flowTuple, ancillary_table[pos].count);
        }
    }
}