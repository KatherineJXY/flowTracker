#ifndef FTRACK_HEADER
#define FTRACK_HEADER

#include "utils/sketch.h"
#include "utils/maxheap.h"
#include "utils/BOBHash32.h"

class FlowTrackerAve : public Sketch
{
private:
    struct mainTableRecords
    {
        int flow_count;
        int sentinel_count;
        double flow_records;
    };

    int flow_filter_size, main_table_size, num_hash, row_size;

    int *flow_filter;
    mainTableRecords *main_table;

    BOBHash32 *flow_filter_hashes, *main_table_hash;
    
public:
    FlowTrackerAve(int flow_filter_size, int main_table_size, int num_hash);
    ~FlowTrackerAve();
    
    void init();
    void status();
    
    void insert(string flow, int val);
    double query_average(string flow);
};

#endif/*FTRACK_HEADER*/
