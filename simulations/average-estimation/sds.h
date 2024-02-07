#ifndef SIMPLE_DELAY_SKETCH_HEADER
#define SIMPLE_DELAY_SKETCH_HEADER

#include "utils/sketch.h"
#include "utils/maxheap.h"
#include "utils/BOBHash32.h"

class SimpleDelaySketch : public Sketch
{
private:
    struct sdsBuks
    {
        int count;  // packet count
        int delay_sum;  // sum of delay
    };
    
    int size, num_hash, row_size;

	sdsBuks *sds_buks;
	BOBHash32 *hash;
    
public:
    SimpleDelaySketch(int size, int num_hash);
    ~SimpleDelaySketch();
    void init();
    void status();
    void insert(string flow, int val);	// x: key  val: value
    double query_average(string flow);
};


#endif /*SIMPLE_DELAY_SKETCH_HEADER*/