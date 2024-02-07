#ifndef IMPL_HEADER
#define IMPL_HEADER

#include <iostream>
#include <vector>
using std::string;
using std::vector;
using std::pair;

typedef pair<string, int> PSI;
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

	virtual void insert(string flow, int val)
	{
		std::cout << "No insert function." << std::endl;
	}

	virtual void remove()
	{
		std::cout << "No remove function." << std::endl;
	}

    virtual int query(string flow)
    {
        std::cout << "No query function." << std::endl;
    }

	virtual double query_average(string flow)
	{
		std::cout << "No query average function." << std::endl;
	}

	virtual void status()
	{
		std::cout << "No status function." << std::endl;
	}
};

#endif