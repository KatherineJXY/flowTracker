#include "cu.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

CUSketch::CUSketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
    sprintf(name, "Half CUSketch");

    if (size <= 0 || num_hash <= 0)
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;

    cnt = new int[size];
    hash = new BOBHash32[num_hash];

    row_size = size / num_hash;
}

CUSketch::~CUSketch()
{
    if (cnt)
        delete [] cnt;
    if (hash)
        delete [] hash;
}

void
CUSketch::init()
{
    memset(cnt, 0, size * sizeof(int));
    flow.clear();

    for (int i = 0; i < num_hash; ++i)
        hash[i].initialize(rand() % MAX_PRIME32);
}

void
CUSketch::status()
{
    printf("bucket: %d  hash: %d\n", size, num_hash);
}

void
CUSketch::insert(int x, int val)
{
    flow.insert(x);

    while (val)
    {
        int minus = val;

        int minp = INT_MAX, secp = INT_MAX;
        int i = 0, base = 0;
        int sav_pos[num_hash];

        for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
        {
            int pos = hash[i].run((char*)&x, sizeof(int)) % row_size + base;
            sav_pos[i] = pos;
            minp = min(minp, cnt[pos]);
        }

        for (i = 0; i < num_hash; ++i)
        {
            int pos = sav_pos[i];
            if (cnt[pos] != minp)
                secp = min(secp, cnt[pos]);
        }

        minus = min(minus, secp-minp);

        for (i = 0; i < num_hash; ++i)
        {
            int pos = sav_pos[i];
            if (cnt[pos] == minp)
                cnt[pos] += minus;
        }

        val -= minus;
    }
}

int
CUSketch::query(int x)
{
    int ans = INT_MAX;

    int i = 0, base = 0;
    for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
        int pos = hash[i].run((char*)&x, sizeof(int)) % row_size + base;
        ans = min(ans, cnt[pos]);
    }

    return ans;
}

vector<PII>
CUSketch::query_all()
{
    vector<PII> total_ans;

    for(auto it = flow.begin(); it != flow.end(); it++)
    {
        int ans = INT_MAX;

        int i = 0, base = 0;
        for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
        {
            int pos = hash[i].run((char*)&it, sizeof(int)) % row_size + base;
            ans = min(ans, cnt[pos]);
        }
        total_ans.push_back(mp((*it), ans));
    }

    return total_ans;
}