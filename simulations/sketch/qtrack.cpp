#include "qtrack.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

QueueTrack::QueueTrack(int ffilter_size, int qrecorder_size, int num_hash) :
ffilter_size(ffilter_size), qrecorder_size(qrecorder_size), num_hash(num_hash)
{
    sprintf(name, "QueueTrack");

    if (ffilter_size <= 0 || num_hash <= 0 || qrecorder_size <= 0)
        cout << "SIZE & NUM_HASH must be POSITIVE intergers." << endl;

    ffil = new int[ffilter_size];
    qr = new QRecorder[qrecorder_size];
    ff_hash = new BOBHash32[num_hash];
    qr_hash = new BOBHash32();
}

QueueTrack::~QueueTrack()
{
    if (ffil)
        delete [] ffil;
    if (qr)
        delete [] qr;
    if (ff_hash)
        delete [] ff_hash;
    if (qr_hash)
        delete qr_hash;
}

void
QueueTrack::init()
{
    memset(ffil, 0, ffilter_size * sizeof(int));
    memset(qr, 0, qrecorder_size * sizeof(struct QRecorder));

    drop = false;
    reside_pos = 0;

    qr_hash->initialize(rand() % MAX_PRIME32);

    for (int i = 0; i < num_hash; ++i)
    {
        ff_hash[i].initialize(rand() % MAX_PRIME32);
    }
}

void
QueueTrack::status()
{
    printf("Flow filter: bucket %d  hash:%d\n", ffilter_size, num_hash);
    printf("Queuing recorder: bucket: %d\n", qrecorder_size);
}

void
QueueTrack::track_pos(int x)
{
    bool flag = true;   // Existance
    int up_ff = 0;      // val to update flow filter

    int sav_pos[num_hash];
    int tmp = 0;
    // cout << "flow filter query: ";
    for (int i = 0; i < num_hash; ++i)
    {
        int pos = ff_hash[i].run((char*)&x, sizeof(int)) % ffilter_size;
        sav_pos[i] = pos;
        if (ffil[pos] == 0) {
            flag = false;
        } 
        tmp += ffil[pos];
        // cout << "ffil[" << pos << "]: " << ffil[pos] << ", ";
    }
    // cout << endl;
    // cout << "flag: " << flag << endl; 
    if (flag == true && tmp <= num_hash) // flow exists: values in corresponding buckets in the flow filter are greater than 1
    {
        flag = false;        
    }

    // cout << "now flag: " << flag << endl;
    int origo = qr_hash->run((char*)&x, sizeof(int)) % qrecorder_size;
    // cout << "origo position: " << origo << endl;

    if (!flag)  // Not exist
    {
        // cout << "Enter not flag!" << endl;
        if (qr[origo].step_width != 0)  // origo bucket is not empty
        {
            // cout << "collision happens! Step width is " << qr[origo].step_width << endl;
            qr[origo].flow_cnt --;
            long long step = ((1 << (qr[origo].step_width)) - 1) % qrecorder_size;
            // long long step = pow(2, qr[origo].step_width) - 1;    // 2^n - 1
            // cout << "Move step " << step << endl;
            reside_pos = (origo + step) % qrecorder_size;
            // cout << "reside position is " << origo << "+" << step << "%" << qrecorder_size << "=" << reside_pos << endl;

            if (qr[reside_pos].step_width == 0)
                qr[reside_pos].step_width ++;
            else 
                drop = true;
        } else {
            reside_pos = origo;
            qr[origo].flow_cnt ++;
        }
        qr[origo].step_width ++;
        
        if (!drop)
            up_ff = qr[origo].step_width + 1;
    }
    else    // exist 
    {
        // cout << "Enter flag!" << endl;
        int ff_val = INT_MIN;

        for (int i = 0; i < num_hash; ++i)
        {
            int pos = sav_pos[i];
            ff_val = max(ffil[pos], ff_val);
        }

        long long step = ((1 << (ff_val -2 )) - 1) % qrecorder_size;
        // long long step = pow(2, (ff_val - 2)) - 1;
        reside_pos = (origo + step) % qrecorder_size;

        if (qr[reside_pos].flow_cnt < 0)
        {
            // cout << "evade from the original position!" << endl;
            // clear the reside position
            up_ff = 1;
            qr[reside_pos] = {0};
            drop = true;
        } else {
            qr[reside_pos].flow_cnt ++;
        }
    }

    // cout << "reside position: " << reside_pos << endl; 
    // cout << "flow filter update value: " << up_ff << endl;
    
    // cout << "updated flow filter: ";
    if (up_ff)
    {
        for (int i = 0; i < num_hash; ++i)
        {
            int pos = sav_pos[i];
            
            if (ffil[pos] == 0 || ffil[pos] > up_ff)
                ffil[pos] = up_ff;
            // cout << "ffil[" << pos << "]: " << ffil[pos] << ", ";
        }
    }
    // cout << endl;
}

void
QueueTrack::insert(int x, int val)
{
    track_pos(x);

    if (!drop)
        qr[reside_pos].cnt += val;
    // cout << "Inserting... " << endl;
    // cout << "qr[" << reside_pos << "]: " << qr[reside_pos].cnt << endl;
}

void
QueueTrack::insert_interval_variation(int x, int timeval)
{
    track_pos(x);

    if (!drop)
    {
        qr[reside_pos].exp = (qr[reside_pos].exp*0.5 + timeval*0.5);
        qr[reside_pos].exp2 = qr[reside_pos].exp2*0.5 + timeval*timeval*0.5;
        qr[reside_pos].cnt = qr[reside_pos].exp2 - qr[reside_pos].exp*qr[reside_pos].exp;
    }
}

void
QueueTrack::insert_interval_average(int x, int timeval)
{
    track_pos(x);
    
    if (!drop)
        qr[reside_pos].cnt = qr[reside_pos].cnt*0.5 + timeval * 0.5;
}

void
QueueTrack::insert_max_interval(int x, int timeval)
{
    track_pos(x);

    if (!drop)
        qr[reside_pos].cnt = max(qr[reside_pos].cnt, timeval);
}

int
QueueTrack::query(int x)
{
    int ans;
    int ff_val = INT_MIN;

    for (int i = 0; i < num_hash; ++i)
    {
        int pos = ff_hash[i].run((char*)&x, sizeof(int)) % ffilter_size;
        ff_val = max(ffil[pos], ff_val);
    }

    int origo = qr_hash->run((char*)&x, sizeof(int)) % qrecorder_size;
    long long step = ((1 << (ff_val - 2)) - 1) % qrecorder_size;
    // long long step = pow(2, (ff_val - 2)) - 1;
    reside_pos = (origo + step) % qrecorder_size;

    ans = qr[reside_pos].cnt;

    // cout << "querying..." << endl;
    // cout << "qr[" << reside_pos << "]: " << qr[reside_pos].cnt << endl;
    return ans;
}