#ifndef CMS_HEADER
#define CMS_HEADER

#include "sketch.h"
#include "maxheap.h"
#include "BOBHash32.h"

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
	void insert(int x, int val);	// x: key  val: value
	int query(int x);
	void status();
};

#endif // CMS_HEADER