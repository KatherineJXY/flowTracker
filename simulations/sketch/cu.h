#ifndef CUS_HEADER
#define CUS_HEADER

#include "sketch.h"
#include "maxheap.h"
#include "BOBHash32.h"

class CUSketch : public Sketch
{
private:
    int size, num_hash, row_size;
    set<int> flow;

    int *cnt;
    BOBHash32 *hash;

public:
    CUSketch(int size, int num_hash);
    ~CUSketch();
    void init();
    void insert(int x, int val);
    int query(int x);
    vector<PII> query_all();
    void status();
};

#endif // CUS_HEADER