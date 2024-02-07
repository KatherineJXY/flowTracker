#ifndef LOSSY_DELAY_SKETCH_HEADER
#define LOSSY_DELAY_SKETCH_HEADER

#include "utils/sketch.h"
#include "utils/maxheap.h"
#include "utils/BOBHash32.h"

class LossyDelaySketch : public Sketch
{
private:
    struct ldSketch
    {
        int count;
        int delay_sum;
        string flow;
    };

    int size, num_hash, row_size;

    ldSketch *lds1, *lds2; // require two sketch: one for sender, one for receiver
    BOBHash32 *hash;

public:
    LossyDelaySketch(int size, int num_hash);
    ~LossyDelaySketch();
    void init();
    void status();
    string xorStrings(const std::string &str1, const std::string &str2);
    void insert(string flow, int val); // x: key  val: value
    double query_average(string flow);
};

#endif /*LOSSY_DELAY_SKETCH_HEADER*/