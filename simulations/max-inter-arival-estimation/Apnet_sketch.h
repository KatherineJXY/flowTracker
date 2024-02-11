#ifndef APNET_SKETCH_HEADER
#define APNET_SKETCH_HEADER

#include "utils/sketch.h"
#include "utils/maxheap.h"
#include "utils/BOBHash32.h"

class ApnetSketch : public Sketch
{
private:
    struct Bulks
    {
        string ip_src;
        string ip_dst;
        string l4_src_port;
        string l4_dst_port;
        string protocol;

        int max_count;
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

	Bulks *bulks;		   
	BOBHash32 *hash;

public:
    ApnetSketch(int size, int num_hash);
    ~ApnetSketch();

    void init();
    fiveTuple segement(string flow);
    bool isEqual(int pos, const fiveTuple& flowTuple);
    bool isEmpty(int pos, const fiveTuple& flowTuple);
    void recordPromotion(int pos, const fiveTuple& flowTuple, int max_count);
	void insert(string flow, int val);	// x: key  val: value
	int query_max(string flow);
	void status();
};

#endif /* APNET_SKETCH_HEADER */
