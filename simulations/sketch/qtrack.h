#ifndef QTRACK_HEADER
#define QTRACK_HEADER

#include "sketch.h"
#include "maxheap.h"
#include "BOBHash32.h"

class QueueTrack : public Sketch
{
private:

    struct QRecorder
    {
        int step_width;
		int flow_cnt;
        int cnt;
		int exp;
		int exp2;
    };
    
	int ffilter_size, num_hash;
	int qrecorder_size;

	// flow details
	bool drop;
	int reside_pos;

	int *ffil;
	QRecorder *qr;
	BOBHash32 *qr_hash, *ff_hash;

public:
	QueueTrack(int ffilter_size, int qrecorder_size, int num_hash);
	~QueueTrack();
	void init();
	void status();
	void track_pos (int x);
	void insert(int x, int val);
    void insert_interval_variation(int x, int timeval);
	void insert_interval_average(int x, int timeval);
	void insert_max_interval(int x, int timeval);
	int query(int x);
};



#endif	// QUTRACK_HEADER