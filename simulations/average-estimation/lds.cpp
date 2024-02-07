#include "lds.h"
#include <cstdio>
#include <map>
#include <cmath>
#include <cstring>
#include <climits>

LossyDelaySketch::LossyDelaySketch(int size, int num_hash):
size(size), num_hash(num_hash)
{
    sprintf(name, "LossyDelaySketch");

	if (size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}
    lds1 = new ldSketch[size];
    lds2 = new ldSketch[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

LossyDelaySketch::~LossyDelaySketch()
{
    if (lds1)
        delete [] lds1;
    if (lds2)
        delete [] lds2;
    if (hash)
        delete [] hash;
}

void
LossyDelaySketch::init()
{
    memset(lds1, 0, size * sizeof(struct ldSketch));
	memset(lds2, 0, size * sizeof(struct ldSketch));

	for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(rand() % MAX_PRIME32);
	}
}

void
LossyDelaySketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

string 
LossyDelaySketch::xorStrings(const std::string& str1, const std::string& str2) {
    std::string result;
    size_t length = std::min(str1.length(), str2.length());
    for (size_t i = 0; i < length; ++i) {
        // 对两个字符串的对应字符进行异或操作，并将结果添加到结果字符串中
        result += (str1[i] ^ str2[i]);
    }
    return result;
}


void
LossyDelaySketch::insert(string flow, int val)
{
    int i = 0, base = 0;

    // for simplify, ingress insertion is set to 0.5*val, egress insertion is set to 1.5*val
    for (i=0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        // insertion to ingress sketch
        lds1[pos].count ++;
        lds1[pos].delay_sum += 0.5*val;
        if (lds1[pos].flow.empty())
        {
            lds1[pos].flow = flow;
        }
        else
        {
            lds1[pos].flow = xorStrings(lds1[pos].flow, flow);
        }

        // insertion to egress sketch
        lds2[pos].count ++;
        lds2[pos].delay_sum += 1.5*val;
        if (lds2[pos].flow.empty())
        {
            lds2[pos].flow = flow;
        }
        else
        {
            lds2[pos].flow = xorStrings(lds2[pos].flow, flow);
        }
	}
}

double 
LossyDelaySketch::query_average(string flow)
{
    int min_cnt = INT_MAX;
    map<int, int> cells;    // delay, flow count

    int i = 0, base = 0;
    for(i = 0, base = 0; i < num_hash; ++i, base += row_size)
    {
		int pos = hash[i].run(flow.c_str(), flow.length()) % row_size + base;
        if (lds1[pos].count == lds2[pos].count && lds1[pos].flow == lds2[pos].flow)
        {
            min_cnt = min(min_cnt, lds1[pos].count);
            cells.insert(make_pair((lds2[pos].delay_sum-lds1[pos].delay_sum), lds1[pos].count));
        }
    }

    int sum_cnt=0, sum_delay=0;
    for(auto & cell : cells)
    {
        if(cell.second < 1.1*min_cnt)
        {
            sum_cnt += cell.second;
            sum_delay += cell.first;
        }
    }

    return double(sum_delay/sum_cnt);
}