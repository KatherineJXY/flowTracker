#ifndef SUMAX_HEADER_H
#define SUMAX_HEADER_H

#include "utils/sketch.h"
#include "utils/maxheap.h"
#include "utils/BOBHash32.h"

class SuMax : public Sketch
{
private:
    int size, num_hash, row_size;

	int *max_cnt;				   
	BOBHash32 *hash;

public:
    SuMax (int size, int num_hash);
    ~SuMax ();

    void init();
	void insert(string flow, int val);	// x: key  val: value
	int query_max(string flow);
	void status();
};

#endif /* SUMAX_HEADER_H */