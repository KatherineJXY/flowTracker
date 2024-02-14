#include "ftrack.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

FlowTracker::FlowTracker(int flow_filter_size, int main_table_size, int num_hash) :
flow_filter_size (flow_filter_size), main_table_size (main_table_size), num_hash (num_hash)
{
    sprintf (name, "FlowTracker");
    
    if (flow_filter_size <= 0 || main_table_size <= 0 || num_hash <= 0)
    {
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
    }
    
    main_table = new mainTableRecords[main_table_size];
    flow_filter = new int[flow_filter_size];
    flow_filter_hashes = new BOBHash32[num_hash];
    main_table_hash = new BOBHash32();
    
    row_size = flow_filter_size / num_hash;
}

FlowTracker::~FlowTracker()
{
    if (main_table)
        delete [] main_table;
    if (flow_filter)
        delete [] flow_filter;
    if (flow_filter_hashes)
        delete [] flow_filter_hashes;
    if (main_table_hash)
        delete main_table_hash;
}

void
FlowTracker::init()
{
    memset(main_table, 0, main_table_size * sizeof(struct mainTableRecords));
    memset(flow_filter, 0, flow_filter_size * sizeof(int));
    
    main_table_hash->initialize(rand() % MAX_PRIME32);
    
    for (int i = 0; i < num_hash; i++)
    {
        flow_filter_hashes[i].initialize(rand() % MAX_PRIME32);
    }
}

void
FlowTracker::status()
{
    printf("Flow filter: counter arrays %d \t hash: %d\n", flow_filter_size, num_hash);
    printf("Main table: buckets %d \n", main_table_size);
}

void
FlowTracker::insert(string flow, int val)
{
    // read flow filter
    int fcnt = INT_MIN;
    int save_pos[num_hash];
    
    int i = 0, base = 0;
    for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
        int pos = flow_filter_hashes[i].run(flow.c_str(), flow.length()) % row_size + base;
        save_pos[i] = pos;
        // cout << "pos " << i << ": " << pos << endl;
        
        if (flow_filter[pos] > fcnt)
            fcnt = flow_filter[pos];
        
        if (flow_filter[pos] == 0 || fcnt == 0)
            fcnt = 0;
    }
    
    int resident_pos = main_table_hash->run(flow.c_str(), flow.length()) % main_table_size;
    int fu = 0;   // to update corresponding counter arrays in the flow filter
    
    if (fcnt == 0 || fcnt == 1) // 0 for new flow, 1 for flow has been evicted
    {
        // cout << "new flow" << endl;
        main_table[resident_pos].sentinel_count ++;
        fu = main_table[resident_pos].sentinel_count + 1;
        
        if(main_table[resident_pos].sentinel_count != 1)  // not empty bucket
        {
            int step_width = (1 << (main_table[resident_pos].sentinel_count - 1)) - 1;
            resident_pos = (resident_pos + step_width) % main_table_size;
            
            if (main_table[resident_pos].flow_count != 0)   // substitute bucket is non-empty
                fu = 0;
            else
                main_table[resident_pos].sentinel_count ++;
        }
        
        if (fu != 0)    // successfully find a resident bucket
            main_table[resident_pos].flow_count ++;
            // cout << "successfully find a resident bucket!";
            // cout << "resident pos: " << resident_pos << "\t sentinel counter: " << main_table[resident_pos].sentinel_count << "\t flow count: " << main_table[resident_pos].flow_count << endl;
    }
    else    // existing flow
    {
        // cout << "existing flow" << endl;
        int step_width = (1 << (fcnt - 2)) - 1;
        resident_pos = (resident_pos + step_width) % main_table_size;
        // cout << "step width is " << step_width << "\t resident pos is " << resident_pos << endl;
        
        main_table[resident_pos].flow_count ++;
        if (main_table[resident_pos].flow_count < main_table[resident_pos].sentinel_count)
        {
            // evade
            main_table[resident_pos].sentinel_count = 0;
            fu = 1;
            cout << "evading!!!" << endl;
        }
    }
    
    // update flow filter
    // cout << "fu: " << fu << endl;
    if (fu != 0)
    {
        // cout << "update flow filter: " << endl;
        for (i = 0; i < num_hash; i++)
        {
            int pos = save_pos[i];
            if (flow_filter[pos] == 0 || flow_filter[pos] > fu) {
                // cout << pos << ": ";
                // cout << flow_filter[pos];
                flow_filter[pos] = fu;
                // cout << "\t" << flow_filter[pos] << endl;
            }
        }
    }
}

int
FlowTracker::query (string flow)
{
    int ans = 0;
    
    // read flow filter
    int fcnt = INT_MIN;
    
    int i = 0, base = 0;
    for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
        int pos = flow_filter_hashes[i].run(flow.c_str(), flow.length()) % row_size + base;
        
        if (flow_filter[pos] > fcnt)
            fcnt = flow_filter[pos];
        
        if (flow_filter[pos] == 0 || fcnt == 0)
            fcnt = 0;
    }
    
    int resident_pos = main_table_hash->run(flow.c_str(), flow.length()) % main_table_size;
    if (fcnt > 1)
    {
        int step_width = (1 << (fcnt - 2)) - 1;
        resident_pos = (resident_pos + step_width) % main_table_size;
        
        // querying flow count
        ans = main_table[resident_pos].flow_count;
    }
    
    return ans;
}
