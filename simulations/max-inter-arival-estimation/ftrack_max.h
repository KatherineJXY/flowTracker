#ifndef FTRACK_MAX_HEADER
#define FTRACK_MAX_HEADER

#include "../utils/sketch.h"
#include "../utils/maxheap.h"
#include "../utils/BOBHash32.h"

class FlowTrackerMax : public Sketch
{
private:
    struct mainTableRecords
    {
        int flow_count;
        int sentinel_count;
        int flow_records;
    };

    int flow_filter_size, main_table_size, num_hash, row_size;

    int *flow_filter;
    mainTableRecords *main_table;

    BOBHash32 *flow_filter_hashes, *main_table_hash;
    
public:
    FlowTrackerMax(int flow_filter_size, int main_table_size, int num_hash);
    ~FlowTrackerMax();
    
    void init();
    void status();
    
    void insert(string flow, int val);
    int query_max(string flow);
};

#endif/*FTRACK_MAX_HEADER*/