#ifndef HASH_FLOW_HEADER
#define HASH_FLOW_HEADER

#include "sketch.h"
#include "maxheap.h"
#include "BOBHash32.h"

class HashFlow : public Sketch
{
private:
    struct mainTableRecords
    {
        string ip_src;
        string ip_dst;
        string l4_src_port;
        string l4_dst_port;
        string protocol;

        int count;
    };

    struct ancillaryTableRecords
    {
        int digest;
        int count;
    };
    
    
    int main_table_size, ancillary_table_size, num_hash;

    mainTableRecords *main_table;
    ancillaryTableRecords *ancillary_table;
    BOBHash32 *main_table_hashes, *ancillary_table_hash;

public:
    HashFlow(int main_table_size, int ancillary_table_size, int num_hash);
    ~HashFlow();

    void init();
    void status();
};

#endif /*HASH_FLOW_HEADER*/