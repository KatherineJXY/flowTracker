#include "ftrack_max.h"
#include "Apnet_sketch.h"
#include "sumax.h"

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

void loadData(char file[], int no, int num_line = -1)
{
    ifstream is(file, ios::in);
    string buf;
    int count = 0;

    while (num_line == -1 || count < num_line)
    {
        if (!getline(is, buf))
        {
            break;
        }
        int del = buf.find(' ');
        string idt = buf.substr(0, del);
        int rc = stoi(buf.substr(del+1));

        flow[no].push_back(make_pair(idt,rc));
        count ++;
    }

    std::cout << file << " Loading complete. " << endl;
    std::cout << "Loading " << flow[no].size() << " of flows. " << endl;
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
    double precision = 0.0, recall = 0.0, f1_score = 0.0;

    for(int j = 1; j <= num_sw; ++j)
    {
        MaxHeap rl_heap(real_ans[j].size());
        MaxHeap est_heap(est_ans[j].size());

        for(auto &it : est_ans[j])
            est_heap.insert(it.first, it.second);

        for(auto &it : real_ans[j])
            rl_heap.insert(it.first, it.second);

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

    precision /= num_sw;
    recall /= num_sw;
    if (precision + recall == 0)
        f1_score = 0;
    else
        f1_score = 2 * (precision * recall) / (precision + recall);

    fstream fout(file_name, ios::out | ios::app);
    fout << "heavy hitter precision: " <<  precision << endl;
    fout << "heavy hitter recall: " << recall << endl;
    fout << "heavy hitter f1-score: " << f1_score << endl << endl;
}

void maxDelayEstimation(vector<Sketch*> sk, vector< pair<string, int> > flow[], string file_name, bool init_first = false)
{
    map<string, int> real_ans[num_sw+1];    // real ans

    for (int j = 1; j <= num_sw; ++j)
    {
        if (init_first)
            sk[j]->init();
        
        for (auto it = flow[j].begin(); it != flow[j].end(); ++it)
        {
            string fid = it->first;
            real_ans[j][fid] = max(real_ans[j][fid], it->second);
            sk[j]->insert(fid, it->second);
        }
    }

    // query after insertion
    map<string, int> est_ans[num_sw+1];
    for (int j = 1; j <= num_sw; ++j)
    {
        for (auto it = real_ans[j].begin(); it != real_ans[j].end(); ++it)
        {
            est_ans[j][it->first] = sk[j]->query_max(it->first);
        }
    }
    // campus : 90percentile 5890  data center : 7369     isp:2761
    heavyHitterDetection(sk, real_ans, est_ans, 5890, file_name);
}

int main(int argc, char *argv[])
{
    srand(2024);
    
    // load data
    string read_file = "../../traces/pcap/campus-pareto.csv";
    std::cout << read_file << endl;

    // heavy hitter estimation
    loadData((char*)read_file.c_str(), num_sw);
    string write_to = "../results/sumax/campus/max-delay-estimation.txt";
    fstream fout(write_to, ios::out | ios::app);

    for (int i = 6; i <= 12; ++i)
    {
        int mem = pow(2, i) * 1024 * 8; // 2^i KB

        vector<Sketch*> sk;

        for (int j = 0; j <= num_sw; ++j)
        {
            // int buk = mem / 96;
            // FlowTrackerMax *ftrack = new FlowTrackerMax(3*buk, buk, 2);
            // sk.push_back(ftrack);
            // int buk = mem / 48;
            // ApnetSketch *apnet = new ApnetSketch(buk, 3);
            // sk.push_back(apnet);
            int buk = mem / 32;
            SuMax *sumax = new SuMax(buk, 3);
            sk.push_back(sumax);
        }

        fout << "The memory usage is 2^" << i << "KB. " << endl;
        std::cout << "The memory usage is 2^" << i << "KB. " << endl;

        maxDelayEstimation(sk, flow, write_to, true);
    }

}