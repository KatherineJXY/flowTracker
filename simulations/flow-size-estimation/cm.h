#ifndef CMS_HEADER
#define CMS_HEADER

#include "../utils/sketch.h"
#include "../utils/maxheap.h"
#include "../utils/BOBHash32.h"

class CMSketch : public Sketch
{
private:
	int size, num_hash, row_size;

	int *cnt;				   
	BOBHash32 *hash;

public:
	CMSketch(int size, int num_hash);
	~CMSketch();
	void init();
	void insert(string flow, int val);	// x: key  val: value
	int query(string flow);
	void status();
};

#endif // CMS_HEADER