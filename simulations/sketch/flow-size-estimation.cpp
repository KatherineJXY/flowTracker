#include "cm.h"
#include "hashflow.h"
#include "ftrack.h"
#include "mv-sketch.h"

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

void heavyHitterDetection(vector<Sketch*> sk, map<string, int> real_ans[], double threshold, string file_name)
{
    double re = 0.0;
    double precision = 0.0, recall = 0.0, f1_score = 0.0;
    double fsc = 0.0;

    for(int j = 1; j <= num_sw; ++j)
    {
        map<string, int> est_ans;
        int size = real_ans[j].size();
        
        for(auto &it : real_ans[j])
            est_ans[it.first]=sk[j]->query(it.first);

        MaxHeap rl_heap(size);
        MaxHeap est_heap(size);

        for(auto &it : est_ans)
            est_heap.insert(it.first, it.second);

        double lc_re = 0.0;
        double lc_fsc = 0.0;
        for(auto &it : real_ans[j])
        {
            rl_heap.insert(it.first, it.second);
            if (est_ans.find(it.first) != est_ans.end())
            {
                lc_fsc ++;
                lc_re += fabs(it.second - est_ans[it.first]) / it.second;
            }
            else
            {
                lc_re += 1.0;
            }
        }

        re += lc_re / real_ans[j].size();
        fsc += lc_fsc / real_ans->size();

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
    fsc /= num_sw;
    precision /= num_sw;
    recall /= num_sw;
    if (precision + recall == 0)
        f1_score = 0;
    else
        f1_score = 2 * (precision * recall) / (precision + recall);
}

void flowSizeEstimatiom(vector<Sketch*> sk, vector< pair<string, int> > flow[], string file_name, bool init_first = false)
{
    map<string, int> real_ans;

    for (int j = 1; j <= num_sw; ++j)
    {
        if (init_first)
            sk[j]->init();
        
        for (auto it = flow[j].begin(); it != flow[j].end(); ++it)
        {
            string fid = it->first;
            real_ans[fid] += 1;
            sk[j]->insert(fid, 1);
        }
    }
}

int main(int argc, char *argv[])
{
    srand(2024);
    
    // load data
    string read_file = "";
    cout << read_file << endl;
    loadData((char*)read_file.c_str(), num_sw);

    string write_to = "";
    fstream fout(write_to, ios::out || ios::app);

    for (int i = 6; i <=12; ++i)
    {
        int mem = pow(2, i) * 1024 * 8; // 2^i KB

        vector<Sketch*> sk;

        for (int j = 0; j <= num_sw; ++j)
        {
            int buk = mem / 32;
            FlowTracker *ftrack = new FlowTracker(mem/4, 3*mem/4, 3);
            sk.push_back(ftrack);
        }

        fout << "The memory usage is 2^" << i << "KB. " << endl;
        cout << "The memory usage is 2^" << i << "KB. " << endl;
    }
}