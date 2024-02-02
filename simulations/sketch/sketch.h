#ifndef IMPL_HEADER
#define IMPL_HEADER

#include <iostream>
#include <vector>
using std::string;
using std::vector;
using std::pair;

typedef pair<int, int> PII;
typedef long long LL;
#define mp std::make_pair
#define ft first
#define sc second

class Sketch
{
public:
    char name[100];

	virtual void init()
	{
		std::cout << "No init function." << std::endl;
	}

	virtual void insert(int x, int val)
	{
		std::cout << "No insert function." << std::endl;
	}

    virtual void insert_interval_variation(int x, int timeval)
    {
        std::cout << "No insert function." << std::endl;
    }

	virtual void insert_interval_average(int x, int timeval)
	{
		std::cout << "No insert function." << std::endl;
	}
	
	virtual void insert_max_interval(int x, int timeval)
	{
		std::cout << "No insert function." << std::endl;
	}

	virtual void remove()
	{
		std::cout << "No remove function." << std::endl;
	}

    virtual int query(int x)
    {
        std::cout << "No query function." << std::endl;
    }

	virtual vector<PII> query_all()
	{
		std::cout << "No query function." << std::endl;
	}

    virtual vector<PII> query_topk(int k)
	{
		std::cout << "No query function." << std::endl;
	}

	virtual vector<PII> query_heavyhitter(int threshold)
	{
		std::cout << "No query function." << std::endl;
	}

	virtual void status()
	{
		std::cout << "No status function." << std::endl;
	}
};

#endif