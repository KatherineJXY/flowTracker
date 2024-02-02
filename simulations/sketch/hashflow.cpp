#include "hashflow.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

HashFlow::HashFlow(int main_table_size, int ancillary_table_size, int num_hash) :
main_table_size(main_table_size), ancillary_table_size(ancillary_table_size), num_hash(num_hash)
{
    sprintf(name, "HashFlow");
    if (main_table_size <= 0 || ancillary_table_size <= 0 || num_hash <= 0)
	{
        cout << "SIZE & NUM_HASH must be POSITIVE integers." << endl;
	}

	main_table = new mainTableRecords[main_table_size];
    ancillary_table = new ancillaryTableRecords[ancillary_table_size];
	main_table_hashes = new BOBHash32[num_hash];
    ancillary_table_hash = new BOBHash32();
}

HashFlow::~HashFlow()
{
    if (main_table)
        delete [] main_table;
    
    if (ancillary_table)
        delete [] ancillary_table;

    if (main_table_hashes)
        delete [] main_table_hashes;
    
    if (ancillary_table_hash)
        delete ancillary_table_hash;
}

void
HashFlow::init()
{
    memset(main_table, 0, main_table_size * sizeof(struct mainTableRecords));
    memset(ancillary_table, 0, ancillary_table_size * sizeof(struct ancillaryTableRecords));

    ancillary_table_hash->initialize(rand() % MAX_PRIME32);

    for (int i = 0; i < num_hash; i++) 
    {
        main_table_hashes[i].initialize(rand() % MAX_PRIME32);
    }
}

