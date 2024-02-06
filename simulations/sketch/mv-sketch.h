#ifndef MV_SKETCH_HEADER
#define MV_SKETCH_HEADER

#include "sketch.h"
#include "maxheap.h"
#include "BOBHash32.h"

class MVSketch : public Sketch
{
private:
    struct Buks
    {
        int total_sum;
        int indicator;

        // flow key
        string ip_src;
        string ip_dst;
        string l4_src_port;
        string l4_dst_port;
        string protocol;
    };

    struct fiveTuple
    {
        string ip_src;
        string ip_dst;
        string l4_src_port;
        string l4_dst_port;
        string protocol;
    };

    int size, num_hash, row_size;

	Buks *buks;				   
	BOBHash32 *hash;
    
public:
    MVSketch(int size, int num_hash);
    ~MVSketch();
    void init();
    void status();
    fiveTuple segement(string flow);    // segement flow string into five tuples
    void insert(string flow, int val);	// x: key  val: value
    bool isEqual(int pos, const fiveTuple& flowTuple); // is flow key equals to that in the bucket indexed by pos
    void recordPromotion(int pos, const fiveTuple& flowTuple);
    int query(string flow);
};

#endif /* MV_SKETCH_HEADER */