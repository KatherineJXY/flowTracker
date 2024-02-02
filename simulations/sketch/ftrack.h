#ifndef FTRACK_HEADER
#define FTRACK_HEADER

#include "sketch.h"
#include "maxheap.h"
#include "BOBHash32.h"

class FlowTracker
{
private:
    struct mainTableRecords
    {
        int flow_cnt;
        int sentinel_count;
        int flow_records;
    };

    int flow_filter_size, main_table_size, num_hash;

    int *flow_filter;
    mainTableRecords *main_table;

    BOBHash32 *flow_filter_hashes, main_table_hash;
    
public:
    FlowTracker(int flow_filter_size, int main_table_size, int num_hash);
    ~FlowTracker();
};

#endif/*FTRACK_HEADER*/