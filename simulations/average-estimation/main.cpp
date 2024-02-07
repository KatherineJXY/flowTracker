#include "ftrack.h"
#include "lds.h"
#include "sds.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdint.h>
#include <algorithm>
#include <iomanip>

using namespace std;

const int num_sw = 1;     // switch
vector< pair <string, int> > flow[num_sw + 1];      // <flow key, packet records>

void loadData(char file[], int no)
{
    ifstream is(file, ios::in);
    string buf;

    while ((getline(is, buf)))
    {
        int del = buf.find(' ');
        string idt = buf.substr(0, del);
        int rc = stoi(buf.substr(del+1));

        flow[no].push_back(make_pair(idt, rc));
    }

    cout << file << " Loading complete. " << endl;
    cout << "Loading " << flow[no].size() << " of flows. " << endl;
}

void topkDetection(vector<Sketch*> sk, map<string, int> real_ans[], map<string, int> est_ans[], int top_k, string file_name)
{
    double precision = 0.0, recall = 0.0, f1_score = 0.0;

    for (int j = 1; j <= num_sw; ++j)
    {
        MaxHeap rl_heap(real_ans[j].size());
        MaxHeap est_heap(est_ans[j].size());

        for (auto &it : real_ans[j])
            rl_heap.insert(it.first, it.second);
        
        for (auto &it : est_ans[j])
            est_heap.insert(it.first, it.second);

        set<string> real_topk = rl_heap.topk(top_k);
        set<string> est_topk = est_heap.topk(top_k);

        double true_positives = 0.0;
        double false_positives = 0.0;
        double false_negatives = 0.0;

        for(const auto& it : est_topk)
        {
            if(real_topk.find(it) != real_topk.end())
                true_positives ++;
            else
                false_positives ++;
        }

        if (true_positives + false_positives == 0)
            precision += 0.0;
        else
            precision += true_positives / (true_positives + false_positives);

        for(const auto& it : real_topk)
            if(est_topk.find(it) == est_topk.end())
                false_negatives ++;
        
        if (real_topk.size() == 0)
            recall += 0;
        else
            recall += true_positives / (true_positives + false_negatives);
    }

    precision /= num_sw;
    recall /= num_sw;
    if (precision + recall == 0)
        f1_score = 0;
    else
        f1_score = 2 * (precision * recall) / (precision + recall);

    fstream fout(file_name, ios::out | ios::app);
    fout << "top-k precision: " <<  precision << endl;
    fout << "top-k recall: " << recall << endl;
    fout << "top-k f1-score: " << f1_score << endl << endl;
}

void heavyHitterDetection(vector<Sketch*> sk, map<string, int> real_ans[], map<string, int> est_ans[], double threshold, string file_name)
{
    double re = 0.0;
    double precision = 0.0, recall = 0.0, f1_score = 0.0;

    for(int j = 1; j <= num_sw; ++j)
    {
        MaxHeap rl_heap(real_ans[j].size());
        MaxHeap est_heap(est_ans[j].size());

        for(auto &it : est_ans[j])
            est_heap.insert(it.first, it.second);

        double lc_re = 0.0;
        for(auto &it : real_ans[j])
        {
            rl_heap.insert(it.first, it.second);
            if (est_ans[j].find(it.first) != est_ans[j].end())
            {
                lc_re += fabs(it.second - est_ans[j][it.first]) / it.second;
            }
            else
            {
                lc_re += 1.0;
            }
        }

        re += lc_re / real_ans[j].size();

        set<string> real_hh = rl_heap.heavy_hitter(threshold);
        set<string> est_hh = est_heap.heavy_hitter(threshold);

        double true_positives = 0.0;
        double false_positives = 0.0;
        double false_negatives = 0.0;

        for(const auto& it : est_hh)
        {
            if(real_hh.find(it) != real_hh.end())
                true_positives ++;
            else
                false_positives ++;
        }

        if (true_positives + false_positives == 0)
            precision += 0.0;
        else
            precision += true_positives / (true_positives + false_positives);

        for(const auto& it : real_hh)
            if(est_hh.find(it) == est_hh.end())
                false_negatives ++;
        
        if (real_hh.size() == 0)
            recall += 0;
        else
            recall += true_positives / (true_positives + false_negatives);
    }
    re /= num_sw;
    precision /= num_sw;
    recall /= num_sw;
    if (precision + recall == 0)
        f1_score = 0;
    else
        f1_score = 2 * (precision * recall) / (precision + recall);

    fstream fout(file_name, ios::out | ios::app);
    fout << "relative error: " << re << endl;
    fout << "heavy hitter precision: " <<  precision << endl;
    fout << "heavy hitter recall: " << recall << endl;
    fout << "heavy hitter f1-score: " << f1_score << endl << endl;
}

void averageDelayEstimation(vector<Sketch*> sk, vector< pair<string, int> > flow[], string file_name, bool init_first = false)
{
    map<string, double> real_ans[num_sw+1];    // real ans
    map<string, int> flow_count[num_sw+1];  // packet count
    map<string, int> delay_sum[num_sw+1];

    for (int j = 1; j <= num_sw; ++j)
    {
        if (init_first)
            sk[j]->init();
        
        for (auto it = flow[j].begin(); it != flow[j].end(); ++it)
        {
            string fid = it->first;
            flow_count[j][fid] ++;
            delay_sum[j][fid] += it->second;
            sk[j]->insert(fid, 1);
        }
    }

    // query after insertion
    map<string, double> est_ans[num_sw+1];
    for (int j = 1; j <= num_sw; ++j)
    {
        for (auto it = flow_count[j].begin(); it != flow_count[j].end(); ++it)
        {
            real_ans[j][it->first] = double(delay_sum[j][it->first] / it->second);
            est_ans[j][it->first] = sk[j]->query_average(it->first);
        }
    }

}